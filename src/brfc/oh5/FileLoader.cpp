/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/oh5/FileLoader.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/QualityGroup.hpp>
#include <brfc/oh5/RootGroup.hpp>
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
    shared_ptr<Group> child = group.child_group_by_name(name);
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

    if (path.dataset_group() != "")
        group = get_or_create_child<DataSetGroup>(*group, path.dataset_group());
    if (path.data_group() != "")
        group = get_or_create_child<DataGroup>(*group, path.data_group());
    if (path.quality_group() != "")
        group = get_or_create_child<QualityGroup>(*group, path.quality_group());
    if (path.attribute_group() != "")
        group = get_or_create_child<AttributeGroup>(*group, path.attribute_group());

    return group;
}

void
FileLoader::add_attribute_from_node(HL_Node* node) {
    QString nodename = QString::fromUtf8(HLNode_getName(node));
    unsigned char* data = HLNode_getData(node);

    SplitPath path(nodename);

    // XXX: don't load if attribute is attached to DataSet
    if (path.dataset() != "")
        return;

    shared_ptr<Attribute> attr =
        make_shared<Attribute>(path.attribute_name());
    
    try {
        const Converter& converter =
            specs_->converter(path.full_attribute_name());
        attr->value(converter.convert(HLNode_getFormat(node), data));
    } catch (const lookup_error& e) {
        // pass
    }

    shared_ptr<Group> group = get_or_create_group(path);
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
