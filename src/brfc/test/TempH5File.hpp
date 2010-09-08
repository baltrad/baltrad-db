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

#ifndef BRFC_TEMPORARY_H5_FILE_HPP
#define BRFC_TEMPORARY_H5_FILE_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/oh5/hlhdf.hpp>

namespace brfc {

class String;
class Variant;

namespace oh5 {

class File;

}

namespace test {

class TempH5File {
  public:
    /**
     * @brief constructor
     *
     * creates a temporary file
     */
    TempH5File();

    /**
     * @brief destructor
     *
     * unlinks the file
     */
    ~TempH5File();

    void unlink();
    
    void write(const oh5::File& f);
    
    String path() const;

    void copy(const String& dest) const;

  private:
    shared_ptr<char> path_;
};

} // namepsace test
} // namespace brfc

#endif // BRFC_TEMPORARY_H5_FILE_HPP
