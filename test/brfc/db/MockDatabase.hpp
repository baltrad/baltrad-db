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

#ifndef BRFC_DB_MOCK_DATABASE_HPP
#define BRFC_DB_MOCK_DATABASE_HPP

#include <gmock/gmock.h>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/Database.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/oh5/PhysicalOh5File.hpp>

namespace brfc {

class MockDatabase : public Database {
  public:
    MOCK_METHOD1(do_is_stored,
        bool(const PhysicalOh5File&));
    MOCK_METHOD1(do_remove,
        bool(const FileEntry&));
    MOCK_METHOD1(do_store,
        FileEntry*(const PhysicalOh5File&));
    MOCK_METHOD1(do_entry_by_file,
        FileEntry*(const PhysicalOh5File&));
    MOCK_METHOD1(do_entry_by_uuid,
        FileEntry*(const std::string&));
    MOCK_METHOD1(do_execute,
        FileResult*(const FileQuery&));
    MOCK_METHOD1(do_execute,
        AttributeResult*(const AttributeQuery&));
    MOCK_CONST_METHOD0(do_sources,
        std::vector<Oh5Source>());
    MOCK_METHOD1(do_add_source,
        void(const Oh5Source&));
    MOCK_METHOD1(do_update_source,
        void(const Oh5Source&));
    MOCK_METHOD1(do_remove_source,
        void(const Oh5Source&));
};

} // namespace brfc

#endif // BRFC_DB_MOCK_DATABASE_HPP
