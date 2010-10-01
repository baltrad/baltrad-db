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

#ifndef BRFC_DB_MOCK_DATABASE_HPP
#define BRFC_DB_MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <brfc/db/Database.hpp>
#include <brfc/db/Query.hpp>

namespace brfc {
namespace db {

class MockDatabase : public Database {
  public:
    MOCK_METHOD1(do_has_file, bool(const oh5::PhysicalFile&));
    MOCK_METHOD1(do_remove_file, bool(const FileEntry&));
    MOCK_METHOD1(do_save_file, shared_ptr<FileEntry>(const oh5::PhysicalFile&));
    MOCK_METHOD1(do_query, shared_ptr<ResultSet>(const Query&));
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_MOCK_DATABASE_HPP
