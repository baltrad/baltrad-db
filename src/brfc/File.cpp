#include <brfc/File.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/Converter.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Source.hpp>
#include <brfc/SplitPath.hpp>
#include <brfc/Variant.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QTime>


namespace brfc {

File::File()
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_()
        , db_id_(0) {

}

File::File(const std::string& path, const AttributeSpecs& specs)
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_(path)
        , db_id_(0) {
    load(path, specs);
}

File::File(const std::string& object,
           const QDate& date,
           const QTime& time,
           const std::string& source,
           const std::string& version)
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_()
        , db_id_(0) {
    root_->add_attribute("Conventions", Variant("ODIM_H5/V2_0"));
    root_->add_attribute("what/object", Variant(object.c_str()));
    root_->add_attribute("what/version", Variant(version.c_str()));
    root_->add_attribute("what/date", Variant(date));
    root_->add_attribute("what/time", Variant(time));
    root_->add_attribute("what/source", Variant(source.c_str()));
}

Source
File::source() const {
    return Source(root_->attribute("what/source").value().qstring());
}

std::string
File::unique_identifier() const {
    QString uid = root_->attribute("what/object").value().qstring();
    uid += "_";
    uid += root_->attribute("what/date").value().date().toString("yyyyMMdd");
    uid += "T";
    uid += root_->attribute("what/time").value().time().toString("hhmmss");
    uid += "_";
    uid += root_->attribute("what/source").value().qstring();
    return uid.toUtf8().constData();
}

File::~File() {

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

    const Variant& val = converter->convert(HLNode_getFormat(node), data);
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
