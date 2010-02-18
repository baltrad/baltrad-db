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

#include <boost/foreach.hpp>

#include <QtCore/QByteArray>
#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTime>


namespace brfc {

File::File()
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_()
        , db_id_(0) {

}

File::File(const QString& path, const AttributeSpecs& specs)
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_(path)
        , db_id_(0) {
    load(path, specs);
}

File::File(const QString& object,
           const QDate& date,
           const QTime& time,
           const QString& source,
           const QString& version)
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_()
        , db_id_(0) {
    root_->add_attribute("Conventions", Variant("ODIM_H5/V2_0"));
    root_->add_attribute("what/object", Variant(object));
    root_->add_attribute("what/version", Variant(version));
    root_->add_attribute("what/date", Variant(date));
    root_->add_attribute("what/time", Variant(time));
    root_->add_attribute("what/source", Variant(source));
}

Source
File::source() const {
    // XXX: return a "default" (null) source if no such attribute?
    //      right now lookup_error is propagated
    const QString& src = root_->attribute("what/source").value().string();
    return Source::from_source_attribute(src);
}

QString
File::unique_identifier() const {
    QString uid = root_->attribute("what/object").value().string();
    uid += "_";
    uid += root_->attribute("what/date").value().date().toString("yyyyMMdd");
    uid += "T";
    uid += root_->attribute("what/time").value().time().toString("hhmmss");
    uid += "_";
    uid += root_->attribute("what/source").value().string();
    return uid;
}

File::~File() {

}

DataObject&
File::data_object(const QString& path,
                  bool create_missing) {
    BRFC_ASSERT(path.startsWith('/'));

    QStringList elems = path.split('/');
    
    elems.pop_front(); // remove root, this is empty as path starts with '/'

    if (path.endsWith('/')) {
        elems.pop_back(); // last element is empty
    }
    
    DataObject* dobj = root_.get();
    BOOST_FOREACH(const QString& elem, elems) {
        dobj = &dobj->child(elem, create_missing);
    }
    return *dobj;
}

void
File::add_attribute_from_node(HL_Node* node,
                              const AttributeSpecs& specs) {
    QString nodename = QString::fromUtf8(HLNode_getName(node));
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
File::load(const QString& path, const AttributeSpecs& specs) {
    init_hlhdflib();
    QByteArray pathbytes = path.toUtf8();
    HL_NodeList* nodes = HLNodeList_read(pathbytes.constData());
    if (nodes == 0) {
        throw fs_error("could not open file: " +
                       std::string(pathbytes.constData()));
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
