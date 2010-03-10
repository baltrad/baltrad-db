#include <brfc/oh5/FileLoader.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Root.hpp>

namespace brfc {
namespace oh5 {

SplitPath::SplitPath(const QString& path,
                     const QStringList& attributegroup_names)
        : attribute_name()
        , group_path() {
    split(path, attributegroup_names);
}

void
SplitPath::split(const QString& path,
                 const QStringList& attributegroup_names) {
    QString sep = QString::fromUtf8("/");
    QStringList elems = path.split(sep);

    BRFC_ASSERT(elems.size() >= 2);

    // last element is always attribute name
    attribute_name = elems.takeLast();

    if (attributegroup_names.contains(elems.last())) {
        full_attribute_name = elems.last() + sep + attribute_name;
    } else {
        full_attribute_name = attribute_name;
    }

    // rest of the element is data object path
    if (elems.size() <= 1) {
        group_path = sep;
    } else {
        group_path = elems.join(sep);
    }
}


FileLoader::FileLoader(const AttributeSpecs* specs)
        : specs_(specs)
        , file_()
        , attributegroup_names_() {
    attributegroup_names_.push_back("what");
    attributegroup_names_.push_back("where");
    attributegroup_names_.push_back("how");
}

FileLoader::~FileLoader() {

}

shared_ptr<Group>
FileLoader::create_group(const QString& name) {
    if (attributegroup_names_.contains(name)) {
        return make_shared<AttributeGroup>(name);
    } else {
        return make_shared<Group>(name);
    }
}

shared_ptr<Group>
FileLoader::get_or_create_group(const QString& path) {
    QStringList names = path.split("/");
    names.removeFirst(); // leading /
    if (names.front() == "")
        names.removeFirst();

    shared_ptr<Group> group = file_->root();

    BOOST_FOREACH(const QString& name, names) {
        shared_ptr<Group> next_group = group->group_by_name(name);
        if (not next_group) {
            next_group = create_group(name);
            group->add_child(next_group);
        }
        group = next_group;
    }

    return group;
}

void
FileLoader::add_attribute_from_node(HL_Node* node) {
    QString nodename = QString::fromUtf8(HLNode_getName(node));
    unsigned char* data = HLNode_getData(node);

    SplitPath path(nodename, attributegroup_names_);

    const Converter* converter = 0;
    const AttributeSpec* spec = 0;
    
    try {
        spec = &specs_->get(path.full_attribute_name);
        converter = &specs_->converter(path.full_attribute_name);
    } catch (const lookup_error& e) {
        file_->ignored_attributes().push_back(nodename);
        return;
    }

    const Variant& value = converter->convert(HLNode_getFormat(node), data);
    shared_ptr<Group> group = get_or_create_group(path.group_path);
    shared_ptr<Attribute> attr = make_shared<Attribute>(path.attribute_name,
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
