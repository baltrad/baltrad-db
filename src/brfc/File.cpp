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
#include <QtCore/QCryptographicHash>


namespace brfc {

File::File()
        : root_(new DataObject("", this))
        , ignored_attributes_()
        , path_()
        , source_()
        , db_id_(0) {

}

shared_ptr<File>
File::from_filesystem(const QString& path, const AttributeSpecs& specs) {
    shared_ptr<File> f(new File);
    f->load(path, specs);
    return f;
}

shared_ptr<File>
File::minimal(const QString& object,
              const QDate& date,
              const QTime& time,
              const QString& source,
              const QString& version) {
    shared_ptr<File> f(new File);
    f->root_->add_attribute("Conventions", Variant("ODIM_H5/V2_0"), true);
    f->root_->add_attribute("what/object", Variant(object));
    f->root_->add_attribute("what/version", Variant(version));
    f->root_->add_attribute("what/date", Variant(date));
    f->root_->add_attribute("what/time", Variant(time));
    f->root_->add_attribute("what/source", Variant(source), true);
    return f;
}

shared_ptr<Source>
File::source() const {
    return source_;
}

void
File::source(shared_ptr<Source> source) {
    source_ = source;
}

QString
File::what_object() const {
    return root_->attribute("what/object").value().string();
}

QDate
File::what_date() const {
    return root_->attribute("what/date").value().date();
}

QTime
File::what_time() const {
    return root_->attribute("what/time").value().time();
}

QString
File::what_source() const {
    return root_->attribute("what/source").value().string();
}

QString
File::unique_identifier() const {
    if (not source_) {
        //XXX: needs a better exception type
        throw value_error("can't form unique_id: not associated with source");
    }
    
    QStringList strs(source_->node_id());
    BOOST_FOREACH(const DataObject& dobj, *root_) {
        BOOST_FOREACH(shared_ptr<Attribute> attr, dobj.attributes()) {
            if (not attr->ignore_in_hash())
                strs.append(attr->to_string());
        }
    }
    strs.sort(); // ensure same order

    QByteArray bytes = strs.join("").toUtf8();
    QByteArray hash = QCryptographicHash::hash(bytes,
                                               QCryptographicHash::Sha1);
    return QString::fromAscii(hash.toHex());
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
    const AttributeSpec* spec = 0;
    
    try {
        spec = &specs.get(path.attribute_name);
        converter = &specs.converter(path.attribute_name);
    } catch (const lookup_error& e) {
        ignored_attributes_.push_back(nodename);
        return;
    }

    const Variant& val = converter->convert(HLNode_getFormat(node), data);
    DataObject& dobj = data_object(path.data_object_path, true);
    dobj.add_attribute(path.attribute_name, val, spec->ignore_in_hash);
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
