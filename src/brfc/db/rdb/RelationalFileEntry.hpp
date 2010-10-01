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

#ifndef BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP
#define BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP

#include <brfc/db/FileEntry.hpp>

namespace brfc {

namespace sql {
    class Connection;
}

namespace db {
namespace rdb {

class RelationalFileEntry : public FileEntry {
  public:
    explicit RelationalFileEntry(shared_ptr<sql::Connection> conn,
                                  long long id=0,
                                  long long lo_id=0);

    virtual ~RelationalFileEntry();

    using FileEntry::id;

    void id(long long id) { id_ = id; }

    void lo_id(long long id) { lo_id_ = id; }

    long long lo_id() const { return lo_id_; }
  
  protected:
    virtual long long do_id() const { return id_; }

    virtual void do_write_to_file(const String& path) const;

  private:
    shared_ptr<sql::Connection> conn_;
    long long id_;
    long long lo_id_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RELATIONAL_FILE_ENTRY_HPP
