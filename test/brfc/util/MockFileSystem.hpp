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

#ifndef BRFC_UTIL_BOOST_FILE_SYSTEM_HPP
#define BRFC_UTIL_BOOST_FILE_SYSTEM_HPP

#include <gmock/gmock.h>

#include <brfc/util/FileSystem.hpp>

namespace brfc {

/**
 * @brief access filesystem using boost::filesystem
 */
class MockFileSystem : public FileSystem {
  public:
    MOCK_CONST_METHOD1(do_exists,
        bool(const std::string&));
    MOCK_CONST_METHOD1(do_is_directory,
        bool(const std::string&));
    MOCK_CONST_METHOD2(do_copy_file,
        void(const std::string&, const std::string&));
    MOCK_CONST_METHOD1(do_remove,
        void(const std::string&));
    MOCK_CONST_METHOD1(do_is_absolute,
        bool(const std::string& path));
    MOCK_CONST_METHOD2(do_join,
        std::string(const std::string&, const std::string&));
    MOCK_CONST_METHOD1(do_file_size,
        long long(const std::string&));
    MOCK_CONST_METHOD1(do_create_directory,
        void(const std::string&));
    MOCK_CONST_METHOD1(do_clear_directory,
        void(const std::string&));
};

} // namespace brfc

#endif // BRFC_UTIL_BOOST_FILE_SYSTEM_HPP
