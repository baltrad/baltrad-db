/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <boost/algorithm/string/erase.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/MemoryNodeBackend.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/Converter.hpp>

namespace brfc {
namespace oh5 {
namespace hl {

HlFile::HlFile()
        : nodes_()
        , path_() {
}

HlFile::HlFile(const std::string& path)
        : nodes_()
        , path_(path) {
    load();
}

HlFile::HlFile(const std::string& object,
               const Date& date,
               const Time& time,
               const std::string& source,
               const std::string& version)
        : nodes_()
        , path_() {
    root().add(new Attribute("Conventions", Scalar("ODIM_H5/V2_0")));
    Node& what = root().add(new Group("what"));
    what.add(new Attribute("object", Scalar(object)));
    what.add(new Attribute("version", Scalar(version)));
    what.add(new Attribute("date", Scalar(date)));
    what.add(new Attribute("time", Scalar(time)));
    what.add(new Attribute("source", Scalar(source)));
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
    parent.add(new Attribute(name, value));
}

void add_node(Node& root, HL_Node* node) {
    std::string path(HLNode_getName(node));
    const std::string nodename = path.substr(path.rfind('/') + 1);
    boost::erase_tail(path, nodename.length() + 1);

    Node* parent = path.empty() ? &root : root.child(path);
    BRFC_ASSERT(parent);

    switch (HLNode_getType(node)) {
        case ATTRIBUTE_ID:
            add_attribute(*parent, nodename, node);
            break;
        case GROUP_ID:
            parent->add(new Group(nodename));
            break;
        case DATASET_ID:
            parent->add(new DataSet(nodename));
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
        add_node(nodes_.root(), node);
    }
}

} // namespace hl
} // namespace oh5
} // namepsace brfc
