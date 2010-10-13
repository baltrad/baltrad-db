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

#ifndef BRFC_DB_RDB_RDB_HELPER_HPP
#define BRFC_DB_RDB_RDB_HELPER_HPP

#include <brfc/sql/Factory.hpp>

namespace brfc {

namespace oh5 {
    class Attribute;
    class Group;
    class Node;
    class PhysicalFile;
    class Source;
}

namespace sql {
    class Connection;
    class Dialect;
    class Result;
}

namespace db {
namespace rdb {

class Model;
class RdbFileEntry;
class RdbNodeBackend;
class RelationalDatabase;

/**
 * @brief gather & simplify database queries
 */
class RdbHelper {
  public:
    explicit RdbHelper(RelationalDatabase* rdb);

    virtual ~RdbHelper();

    sql::Connection& conn();

    const sql::Dialect& dialect();

    RelationalDatabase& rdb() { return *rdb_; }

    RdbNodeBackend& backend(oh5::Node& node) const;

    const RdbNodeBackend& backend(const oh5::Node& node) const;

    virtual void insert_group(oh5::Group& node);

    virtual void insert_attribute(oh5::Attribute& node);

    virtual void insert_node(oh5::Node& node);
    
    virtual void insert_file(RdbFileEntry& entry,
                             const oh5::PhysicalFile& file);

    virtual void insert_file_content(RdbFileEntry& entry, const String& path);

    virtual long long select_source_id(const oh5::Source& source);

    virtual long long select_root_id(const RdbFileEntry& entry);

    virtual oh5::Source select_source(long long id);

    virtual void load_file(RdbFileEntry& entry);

    virtual void load_children(oh5::Node& node);

  private:
    long long last_id(sql::Result& result);

    RelationalDatabase* rdb_;
    const Model& m_;
    sql::Factory sql_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RDB_HELPER_HPP
