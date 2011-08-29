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

#include <brfc/rdb/SaveFile.hpp>

#include <memory>

#include <boost/foreach.hpp>

#include <brfc/FileHasher.hpp>
#include <brfc/rdb/RdbHelper.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>
#include <brfc/rdb/RelationalDatabase.hpp>
#include <brfc/oh5/Oh5Node.hpp>
#include <brfc/oh5/Oh5PhysicalFile.hpp>
#include <brfc/sql/Connection.hpp>

namespace brfc {

SaveFile::SaveFile(RelationalDatabase* rdb)
        : rdb_(rdb) {
}

RdbFileEntry*
SaveFile::operator()(const Oh5PhysicalFile& file) {
    std::auto_ptr<RdbFileEntry> entry_(new RdbFileEntry(rdb_));
    entry_.reset(new RdbFileEntry(rdb_));
    entry_->hash(rdb_->file_hasher().hash(file));
    entry_->loaded(true);

    Oh5NodeBackend& be = entry_->root().backend();

    // copy nodes from file to entry
    Oh5Node::const_iterator iter = file.root().begin();
    Oh5Node::const_iterator end = file.root().end();
    ++iter; // skip root;
    for ( ; iter != end; ++iter) {
        be.add(iter->parent()->path(), iter->clone());
    }

    boost::shared_ptr<sql::Connection> conn = rdb_->conn();
    
    conn->begin();
    try { 
        RdbHelper helper(conn);
        helper.insert_file(*entry_, file);
        helper.insert_file_content(*entry_, file.path());
        helper.insert_nodes(entry_->id(), entry_->root());
        conn->commit();
    } catch (...) {
        conn->rollback();
        throw;
    }

    return entry_.release();
}

} // namespace brfc
