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

#include <brfc/db/rdb/SaveFile.hpp>

#include <boost/foreach.hpp>

#include <brfc/assert.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/PhysicalFile.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace db {
namespace rdb {

SaveFile::SaveFile(RdbFileEntry& entry)
        : entry_(entry)
        , helper_(entry.rdb().helper()) {
}

void
SaveFile::operator()(const oh5::RootGroup& root) {
    helper_.insert_node(entry_.root());
}

void
SaveFile::operator()(const oh5::Group& group) {
    const String& path = group.parent()->path();
    oh5::Node* parent = entry_.node(path);
    BRFC_ASSERT(parent);

    parent->create_group(group.name());
}

void
SaveFile::operator()(const oh5::DataSet& dataset) {
    const String& path = dataset.parent()->path();
    oh5::Node* parent = entry_.node(path);
    BRFC_ASSERT(parent);

    parent->create_dataset(dataset.name());
}

void
SaveFile::operator()(const oh5::Attribute& attr) {
    const String& path = attr.parent()->path();
    oh5::Node* parent = entry_.node(path);
    BRFC_ASSERT(parent);

    parent->create_attribute(attr.name(), attr.value());
}

void
SaveFile::operator()(const oh5::PhysicalFile& file) {
    helper_.insert_file(entry_, file);
    helper_.insert_file_content(entry_, file.path());

    BOOST_FOREACH(const oh5::Node& node, file.root()) {
        visit(node, *this);
    }
}

} // namespace rdb
} // namespace db
} // namespace brfc
