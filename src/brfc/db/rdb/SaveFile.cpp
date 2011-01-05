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

#include <brfc/db/rdb/RdbHelper.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/PhysicalFile.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

#include <brfc/sql/Connection.hpp>

namespace brfc {
namespace db {
namespace rdb {

SaveFile::SaveFile(RelationalDatabase* rdb)
        : rdb_(rdb)
        , entry_() {
}

oh5::Node&
SaveFile::parent_on_entry(const oh5::Node& node) {
    const std::string& path = node.parent()->path();
    oh5::Node* parent = entry_->node(path);
    BRFC_ASSERT(parent);
    return *parent;
}

void
SaveFile::operator()(const oh5::RootGroup& root) {
    // no-op
}

void
SaveFile::operator()(const oh5::Group& group) {
    // XXX: set loaded to prevent (failing) child lookup?
    parent_on_entry(group).create_group(group.name());
}

void
SaveFile::operator()(const oh5::DataSet& dataset) {
    parent_on_entry(dataset).create_dataset(dataset.name()); 
}

void
SaveFile::operator()(const oh5::Attribute& attr) {
    parent_on_entry(attr).create_attribute(attr.name(), attr.value());
}

RdbFileEntry*
SaveFile::operator()(const oh5::PhysicalFile& file) {
    entry_.reset(new RdbFileEntry(rdb_));
    entry_->hash(rdb_->file_hasher().hash(file));
    entry_->loaded(true);

    // copy nodes from file to entry
    BOOST_FOREACH(const oh5::Node& node, file.root()) {
        visit(node, *this);
    }

    shared_ptr<sql::Connection> conn = rdb_->conn();

    conn->begin();
    try { 
        RdbHelper helper(conn);

        helper.insert_file(*entry_, file);
        helper.insert_file_content(*entry_, file.path());

        BOOST_FOREACH(oh5::Node& node, entry_->root()) {
            helper.insert_node(node);
        }

        conn->commit();
    } catch (...) {
        conn->rollback();
        throw;
    }

    return entry_.release();
}

} // namespace rdb
} // namespace db
} // namespace brfc
