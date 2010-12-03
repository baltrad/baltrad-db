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

#include <brfc/oh5/hl/HlFile.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/StringList.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/MemoryNodeBackend.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/Converter.hpp>

namespace brfc {
namespace oh5 {
namespace hl {

HlFile::HlFile()
        : root_(this)
        , path_() {
    root_.backend(new MemoryNodeBackend());
}

HlFile::HlFile(const std::string& path)
        : root_(this)
        , path_(path) {
    root_.backend(new MemoryNodeBackend());
    load();
}

HlFile::HlFile(const std::string& object,
               const Date& date,
               const Time& time,
               const std::string& source,
               const std::string& version)
        : root_(this)
        , path_() {
    root_.backend(new MemoryNodeBackend());
    root().create_attribute("Conventions", Scalar("ODIM_H5/V2_0"));
    Group& what = root().create_group("what");
    what.create_attribute("object", Scalar(object));
    what.create_attribute("version", Scalar(version));
    what.create_attribute("date", Scalar(date));
    what.create_attribute("time", Scalar(time));
    what.create_attribute("source", Scalar(source));
}

HlFile::~HlFile() {

}

namespace {

void add_attribute(Node& parent, const std::string& name, HL_Node* node) {
    shared_ptr<const Converter> converter =
        Converter::create_from_hlhdf_node(*node);
    if (not converter)
        throw std::runtime_error(std::string("could not convert ") +
                                 HLNode_getName(node) + 
                                 " value");
    Scalar value = converter->convert(*node);
    parent.create_attribute(name, value);
}

void add_node(Group& root, HL_Node* node) {
    std::string pathname(HLNode_getName(node));
    StringList path = StringList::split(pathname, "/", StringList::SKIP_EMPTY_PARTS);

    std::string nodename = path.take_last();
    
    Node* parent = 0;
    if (path.empty())
        parent = &root;
    else
        parent = root.child(path.join("/"));
    BRFC_ASSERT(parent);

    switch (HLNode_getType(node)) {
        case ATTRIBUTE_ID:
            add_attribute(*parent, nodename, node);
            break;
        case GROUP_ID:
            parent->create_group(nodename);
            break;
        case DATASET_ID:
            parent->create_dataset(nodename);
            break;
        default:
            break;
    }
}

} // namespace anonymous

void
HlFile::load() {
    init_hlhdflib();

    std::string path_utf8 = path();
    shared_ptr<HL_NodeList> nodes(HLNodeList_read(path_utf8.c_str()),
                                  &HLNodeList_free);
    if (not nodes)
        throw fs_error("could not open file: " + path_utf8);

    HLNodeList_selectAllNodes(nodes.get());
    HLNodeList_fetchMarkedNodes(nodes.get());

    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes.get()); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes.get(), i);
        add_node(root_, node);
    }
}

} // namespace hl
} // namespace oh5
} // namepsace brfc
