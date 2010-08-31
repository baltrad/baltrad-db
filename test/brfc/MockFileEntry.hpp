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
#ifndef BRFC_MOCK_FILE_ENTRY_HPP
#define BRFC_MOCK_FILE_ENTRY_HPP

#include <gmock/gmock.h>

namespace brfc {

class MockFileEntry : public FileEntry {
  public:
    explicit MockFileEntry(long long id)
            : FileEntry(id) {
    }
};

} // namespace brfc

#endif // BRFC_MOCK_FILE_ENTRY_HPP
