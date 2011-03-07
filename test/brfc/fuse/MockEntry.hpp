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

#ifndef BRFC_FUSE_MOCK_ENTRY_HPP
#define BRFC_FUSE_MOCK_ENTRY_HPP

#include <gmock/gmock.h>

#include <brfc/fuse/Entry.hpp>

namespace brfc {
namespace fuse {

class MockEntry : public Entry {
  public:
    MOCK_METHOD1(do_stat,
                 int(struct stat&));
    MOCK_METHOD4(do_readdir,
                 int(void*, fuse_fill_dir_t, off_t, fuse_file_info&));
    MOCK_METHOD1(do_open,
                 int(fuse_file_info&));
    MOCK_METHOD4(do_read,
                 int(char*, size_t, off_t, fuse_file_info&));
    MOCK_METHOD1(do_close,
                 int(fuse_file_info&));
    MOCK_CONST_METHOD0(do_is_up_to_date, bool());
    MOCK_METHOD0(do_update, void());
    MOCK_METHOD1(do_child, Entry*(const std::string&));
};

} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_MOCK_ENTRY_HPP
