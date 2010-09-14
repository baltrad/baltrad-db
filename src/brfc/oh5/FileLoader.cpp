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
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/oh5/FileLoader.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/hlhdf.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Scalar.hpp>

namespace brfc {
namespace oh5 {

namespace {

void add_attribute_from_node(RootGroup& root, HL_Node* node) {
    String nodename = String::from_utf8(HLNode_getName(node));
    StringList path = nodename.split("/");

    String attr_name = path.take_last(); // last element is always attribute

    // XXX: don't load if attribute is attached to DataSet
    if (path.back() == "data")
        return;
     
    shared_ptr<const Converter> converter =
        Converter::create_from_hlhdf_node(*node);
    if (not converter)
        throw std::runtime_error("could not convert " +
                                 nodename.to_utf8() + 
                                 " value");

    Scalar value = converter->convert(*node);

    // create attribute with empty value
    shared_ptr<Attribute> attr = make_shared<Attribute>(attr_name, value);

    path.take_first(); // discard the root element
    shared_ptr<Group> group =
        root.get_or_create_child_group_by_path(path);

    group->add_child(attr);
}

} // namespace anonymous

shared_ptr<File>
FileLoader::load(const String& path) {
    init_hlhdflib();

    std::string path_utf8 = path.to_utf8();
    shared_ptr<HL_NodeList> nodes(HLNodeList_read(path_utf8.c_str()),
                                  &HLNodeList_free);
    if (not nodes)
        throw fs_error("could not open file: " + path_utf8);

    HLNodeList_selectMetadataNodes(nodes.get());
    HLNodeList_fetchMarkedNodes(nodes.get());

    shared_ptr<File> file = File::create();
    file->path(path);
    RootGroup& root = *file->root();

    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes.get()); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes.get(), i);
        if (HLNode_getType(node) == ATTRIBUTE_ID) {
            add_attribute_from_node(root, node);
        }
    }

    return file;
}

} // namespace oh5
} // namespace brfc
