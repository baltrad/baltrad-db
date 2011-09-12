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

#include <brfc/oh5/hl/Oh5HlFileReader.hpp>

#include <boost/algorithm/string/erase.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/HlConverter.hpp>

namespace brfc {

namespace {

void add_attribute(Oh5Node& parent, const std::string& name, HL_Node* node) {
    boost::shared_ptr<const HlConverter> converter =
        HlConverter::create_from_hlhdf_node(*node);
    if (not converter)
        throw std::runtime_error(std::string("could not convert ") +
                                 HLNode_getName(node) + 
                                 " value");
    Oh5Scalar value = converter->convert(*node);
    parent.add(new Oh5Attribute(name, value));
}

void add_node(Oh5Node& root, HL_Node* node) {
    std::string path(HLNode_getName(node));
    const std::string nodename = path.substr(path.rfind('/') + 1);
    boost::erase_tail(path, nodename.length() + 1);

    Oh5Node* parent = path.empty() ? &root : root.child(path);
    BRFC_ASSERT(parent);

    switch (HLNode_getType(node)) {
        case ATTRIBUTE_ID:
            add_attribute(*parent, nodename, node);
            break;
        case GROUP_ID:
            parent->add(new Oh5Group(nodename));
            break;
        case DATASET_ID:
            parent->add(new Oh5DataSet(nodename));
            break;
        default:
            break;
    }
}

} // namespace anonymous


void
Oh5HlFileReader::do_read(const std::string& from, Oh5File& to) const {
    init_hlhdflib();
    boost::shared_ptr<HL_NodeList> nodes(HLNodeList_read(from.c_str()),
                                         &HLNodeList_free);
    if (not nodes)
        throw fs_error("could not open file: " + from);

    HLNodeList_selectAllNodes(nodes.get());
    HLNodeList_fetchMarkedNodes(nodes.get());

    Oh5Node& root = to.root();
    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes.get()); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes.get(), i);
        add_node(root, node);
    }
}


} // namespace brfc
