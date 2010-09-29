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

#include <brfc/oh5/hl/File.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/StringList.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Scalar.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/Converter.hpp>

namespace brfc {
namespace oh5 {
namespace hl {

File::File()
        : root_(this)
        , path_() {
}

File::File(const String& path)
        : root_(this)
        , path_(path) {
    load();
}

File::File(const String& object,
           const Date& date,
           const Time& time,
           const String& source,
           const String& version)
        : root_(this)
        , path_() {
    root().create_attribute("Conventions", Scalar("ODIM_H5/V2_0"));
    Group& what = root().create_group("what");
    what.create_attribute("object", Scalar(object));
    what.create_attribute("version", Scalar(version));
    what.create_attribute("date", Scalar(date));
    what.create_attribute("time", Scalar(time));
    what.create_attribute("source", Scalar(source));
}

File::~File() {

}

namespace {

void add_attribute_from_node(Group& root, HL_Node* node) {
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

    path.take_first(); // discard the root element
    Group& grp = root.get_or_create_child_group_by_path(path);
    grp.create_attribute(attr_name, value);
}

} // namespace anonymous

void
File::load() {
    init_hlhdflib();

    std::string path_utf8 = path().to_utf8();
    shared_ptr<HL_NodeList> nodes(HLNodeList_read(path_utf8.c_str()),
                                  &HLNodeList_free);
    if (not nodes)
        throw fs_error("could not open file: " + path_utf8);

    HLNodeList_selectMetadataNodes(nodes.get());
    HLNodeList_fetchMarkedNodes(nodes.get());

    for (int i=0; i < HLNodeList_getNumberOfNodes(nodes.get()); ++i) {
        HL_Node* node = HLNodeList_getNodeByIndex(nodes.get(), i);
        if (HLNode_getType(node) == ATTRIBUTE_ID) {
            add_attribute_from_node(root_, node);
        }
    }
}

} // namespace hl
} // namespace oh5
} // namepsace brfc
