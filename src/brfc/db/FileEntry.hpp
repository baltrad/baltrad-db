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
#ifndef BRFC_DB_FILE_ENTRY_HPP
#define BRFC_DB_FILE_ENTRY_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

class String;

namespace oh5 {

class File;

} // namespace oh5

namespace db {

/**
 * @brief oh5::File entry in Database
 */
class FileEntry {
  public:
    virtual ~FileEntry() { }

    /**
     * @brief file id
     */
    long long id() const {
        return do_id();
    }
    
    void write_to_file(const String& path) const {
        do_write_to_file(path);
    }

  protected:
    virtual long long do_id() const = 0;

    virtual void do_write_to_file(const String& path) const = 0;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_FILE_ENTRY_HPP
