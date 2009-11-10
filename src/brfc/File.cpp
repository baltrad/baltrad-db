#include <brfc/File.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/hlhdf.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/Converter.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Source.hpp>
#include <brfc/SplitPath.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QVariant>

namespace brfc {

File::File()
        : root_(new DataObject("", this)) {

}

File::File(const std::string& path, const AttributeMapper& mapper)
        : root_(new DataObject("", this)) {
    load(path, mapper);
}

File::File(const std::string& object,
           const std::string& version,
           const QDate& date,
           const QTime& time,
           const std::string& source)
        : root_(new DataObject("", this)) {
    root_->add_attribute("Conventions", "ODIM_H5/V2_0");
    root_->add_attribute("object", QString::fromStdString(object));
    root_->add_attribute("version", QString::fromStdString(version));
    root_->add_attribute("date", date);
    root_->add_attribute("time", time);
    root_->add_attribute("source", QString::fromStdString(source));
}

Source
File::source() const {
    return Source(root_->attribute("source").value().toString());
}

void
File::set_path_from_attributes() {
    QString path = root_->attribute("object").value().toString();
    path += "_";
    path += root_->attribute("date").value().toDate().toString("yyyyMMdd");
    path += "T";
    path += root_->attribute("time").value().toTime().toString("hhmmss");
    path += ".h5";
    root_->add_attribute("path", path);
}

File::~File() {

}

std::string
File::path() const {
    /* XXX: this is a dirty hack
     * we don't want to set path at loading because we might not
     * have all the required attributes (we create non-conforming files
     * for testing purposes).
     */
    if (!root_->has_attribute("path")) {
        File* self = const_cast<File*>(this);
        self->set_path_from_attributes();
    }
    return root_->attribute("path").value().toString().toStdString();
}

DataObject&
File::data_object(const std::string& path,
                      bool create_missing) {
    BRFC_ASSERT(path.find('/') == 0);

    std::vector<std::string> path_vec;
    boost::split(path_vec, path, boost::is_any_of("/"));

    path_vec.erase(path_vec.begin()); // remove root

    if (path.rfind('/') == path.size() - 1) {
        path_vec.pop_back();
    }
    
    DataObject* dobj = root_.get();
    BOOST_FOREACH(const std::string& element, path_vec) {
        dobj = &dobj->child(element, create_missing);
    }
    return *dobj;
}

void
add_attribute_from_node(File& file,
                        HL_Node* node,
                        const AttributeMapper& mapper) {
    std::string nodename = HLNode_getName(node);
    unsigned char* data = HLNode_getData(node);

    SplitPath path(nodename);

    const Converter* converter = 0;
    
    try {
        converter = &mapper.converter(path.attribute_name);
    } catch (const lookup_error& e) {
        // silently ignore an unknown attribute
        return;
    }

    QVariant val = converter->convert(HLNode_getFormat(node), data);
    DataObject& dobj = file.data_object(path.data_object_path, true);
    try {
        dobj.add_attribute(path.attribute_name, val);
    } catch (std::runtime_error& e) {

    }
}

void
File::load(const std::string& path, const AttributeMapper& mapper) {
    init_hlhdflib();
    HL_NodeList* nodes = HLNodeList_read(path.c_str());
    if (nodes == 0) {
        throw fs_error("could not open file: " + path);
    }
    HLNodeList_selectMetadataNodes(nodes);
    HLNodeList_fetchMarkedNodes(nodes);


    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes, i);
        if (HLNode_getType(node) == ATTRIBUTE_ID) {
            add_attribute_from_node(*this, node, mapper);
        }
    }
}

void
File::write(const std::string& path,
            const AttributeMapper& mapper) const {
    init_hlhdflib();
    HL_NodeList* nodes = HLNodeList_new();
    if (nodes == 0) {
        throw brfc_error("could not allocate HL_NodeList");
    }
    
    HL_Node* node = 0;
    BOOST_FOREACH(const DataObject& dobj, root()) {
        const std::string& node_path = dobj.path();
        if (&dobj != root_.get()) { // root group is automatic
            node = HLNode_newGroup(node_path.c_str());
            if (node == 0)
                throw brfc_error("could not create dataset: " + node_path);
            if (HLNodeList_addNode(nodes, node) == 0)
                throw brfc_error("could not add node: " + node_path);
        }
        BOOST_FOREACH(const Attribute& attr, dobj.attributes()) {
            node = HLNode_newAttribute(attr.path().c_str());
            const Converter& c = mapper.converter(attr.name());
            HL_Data d = c.convert(attr.value());
            HLNodeList_addNode(nodes, node);
            HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
        }
    }

    HL_Compression compression;
    HLCompression_init(&compression, CT_ZLIB);
    compression.level = 6;
    if (HLNodeList_setFileName(nodes, path.c_str()) == 0)
        throw brfc_error("could not set filename");
    if (HLNodeList_write(nodes, 0, &compression) == 0)
        throw brfc_error("could not write file");

    HLNodeList_free(nodes);
}

} // namepsace brfc
