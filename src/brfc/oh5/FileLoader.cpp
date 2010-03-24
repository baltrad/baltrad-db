#include <brfc/oh5/FileLoader.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/Data.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Quality.hpp>
#include <brfc/oh5/Root.hpp>
#include <brfc/oh5/SplitPath.hpp>

namespace brfc {
namespace oh5 {


FileLoader::FileLoader(const AttributeSpecs* specs)
        : specs_(specs)
        , file_() {

}

FileLoader::~FileLoader() {

}

namespace {

template<typename T>
shared_ptr<Group>
get_or_create_child(Group& group, const QString& name) {
    shared_ptr<Group> child = group.group_by_name(name);
    if (not child) {
        child = make_shared<T>(name);
        group.add_child(child);
    }
    return child;
}

} // namespace anonymous

shared_ptr<Group>
FileLoader::get_or_create_group(const SplitPath& path) {
    shared_ptr<Group> group = file_->root();

    if (path.dataset() != "")
        group = get_or_create_child<DataSet>(*group, path.dataset());
    if (path.data() != "")
        group = get_or_create_child<Data>(*group, path.data());
    if (path.quality() != "")
        group = get_or_create_child<Quality>(*group, path.quality());
    if (path.attribute_group() != "")
        group = get_or_create_child<AttributeGroup>(*group, path.attribute_group());

    return group;
}

void
FileLoader::add_attribute_from_node(HL_Node* node) {
    QString nodename = QString::fromUtf8(HLNode_getName(node));
    unsigned char* data = HLNode_getData(node);

    SplitPath path(nodename);

    const Converter* converter = 0;
    const AttributeSpec* spec = 0;
    
    try {
        spec = &specs_->get(path.full_attribute_name());
        converter = &specs_->converter(path.full_attribute_name());
    } catch (const lookup_error& e) {
        file_->ignored_attributes().push_back(nodename);
        return;
    }

    const Variant& value = converter->convert(HLNode_getFormat(node), data);
    shared_ptr<Group> group = get_or_create_group(path);
    shared_ptr<Attribute> attr = make_shared<Attribute>(path.attribute_name(),
                                                        value,
                                                        spec->ignore_in_hash);
    group->add_child(attr);
}

shared_ptr<File>
FileLoader::load(const QString& path) {
    init_hlhdflib();
    QByteArray pathbytes = path.toUtf8();
    HL_NodeList* nodes = HLNodeList_read(pathbytes.constData());
    if (nodes == 0) {
        throw fs_error("could not open file: " +
                       std::string(pathbytes.constData()));
    }
    HLNodeList_selectMetadataNodes(nodes);
    HLNodeList_fetchMarkedNodes(nodes);

    file_ = File::create();
    file_->path(path);

    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes, i);
        if (HLNode_getType(node) == ATTRIBUTE_ID) {
            add_attribute_from_node(node);
        }
    }

    return file_;
}


} // namespace oh5
} // namespace brfc
