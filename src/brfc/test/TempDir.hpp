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

#ifndef BRFC_TEST_TEMP_DIR_HPP
#define BRFC_TEST_TEMP_DIR_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

class String;

namespace test {

class TempDir {
  public:
    TempDir();
    ~TempDir();
    std::string path() const;

    void unlink();

  private:
    void rmdir_recurse(const char* path);

    shared_ptr<char> path_;
};

}
} // namespace brfc

#endif // BRFC_TEST_TEMP_DIR_HPP
