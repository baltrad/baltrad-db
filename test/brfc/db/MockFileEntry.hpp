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
#ifndef BRFC_DB_MOCK_FILE_ENTRY_HPP
#define BRFC_DB_MOCK_FILE_ENTRY_HPP

#include <gmock/gmock.h>

#include <brfc/DateTime.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>
#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

class MockFileEntry : public FileEntry {
  public:
    MOCK_CONST_METHOD0(do_uuid, std::string());
    MOCK_CONST_METHOD1(do_write_to_file, void(const std::string&));
    MOCK_CONST_METHOD0(do_source, Oh5Source());
    MOCK_CONST_METHOD0(do_hash, std::string());
    MOCK_CONST_METHOD0(do_size, long long());
    MOCK_CONST_METHOD0(do_stored_at, DateTime());
    MOCK_CONST_METHOD0(do_metadata, const Oh5Metadata&());
};

} // namespace brfc

#endif // BRFC_DB_MOCK_FILE_ENTRY_HPP
