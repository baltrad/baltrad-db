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

#ifndef BRFC_DB_RDB_MOCK_RDB_HELPER_HPP
#define BRFC_DB_RDB_MOCK_RDB_HELPER_HPP

#include <gmock/gmock.h>

#include <brfc/db/rdb/RdbHelper.hpp>

namespace brfc {
namespace db {
namespace rdb {

class MockRdbHelper : public RdbHelper {
  public:
    MockRdbHelper(sql::Connection* conn,
                  FileHasher* hasher)
            : RdbHelper(conn, hasher) {
    }

    MOCK_METHOD1(insert_node, void(oh5::Node&));
    MOCK_METHOD1(insert_attribute, void(oh5::Attribute&));
 
    MOCK_METHOD2(insert_file, void(RdbFileEntry&,
                                   const oh5::PhysicalFile&));

    MOCK_METHOD2(insert_file_content, void(RdbFileEntry&,
                                           const String&));

    MOCK_METHOD1(select_source_id, long long(const oh5::Source&));
    MOCK_METHOD1(select_root_id, long long(const RdbFileEntry&));
    MOCK_METHOD1(select_source, oh5::Source(long long));
    MOCK_METHOD1(load_file, void(RdbFileEntry&));
    MOCK_METHOD1(load_children, void(oh5::Node&));

};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_MOCK_RDB_HELPER_HPP
