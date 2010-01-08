#include <brfc/File.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/AttributeSpecs.hpp>
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

File::File(const std::string& path, const AttributeSpecs& specs)
        : root_(new DataObject("", this)) {
    load(path, specs);
}

File::File(const std::string& object,
           const std::string& version,
           const QDate& date,
           const QTime& time,
           const std::string& source)
        : root_(new DataObject("", this)) {
    root_->add_attribute("Conventions", "ODIM_H5/V2_0");
    root_->add_attribute("what/object", QString::fromStdString(object));
    root_->add_attribute("what/version", QString::fromStdString(version));
    root_->add_attribute("what/date", date);
    root_->add_attribute("what/time", time);
    root_->add_attribute("what/source", QString::fromStdString(source));
}

Source
File::source() const {
    return Source(root_->attribute("what/source").value().toString());
}

void
File::set_path_from_attributes() {
    QString path = root_->attribute("what/object").value().toString();
    path += "_";
    path += root_->attribute("what/date").value().toDate().toString("yyyyMMdd");
    path += "T";
    path += root_->attribute("what/time").value().toTime().toString("hhmmss");
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
File::add_attribute_from_node(HL_Node* node,
                              const AttributeSpecs& specs) {
    std::string nodename = HLNode_getName(node);
    unsigned char* data = HLNode_getData(node);

    SplitPath path(nodename);

    const Converter* converter = 0;
    
    try {
        converter = &specs.converter(path.attribute_name);
    } catch (const lookup_error& e) {
        ignored_attributes_.push_back(nodename);
        return;
    }

    QVariant val = converter->convert(HLNode_getFormat(node), data);
    DataObject& dobj = data_object(path.data_object_path, true);
    try {
        dobj.add_attribute(path.attribute_name, val);
    } catch (std::runtime_error& e) {

    }
}

void
File::load(const std::string& path, const AttributeSpecs& specs) {
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
            add_attribute_from_node(node, specs);
        }
    }
}

} // namepsace brfc
