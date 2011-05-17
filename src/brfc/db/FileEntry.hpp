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
#ifndef BRFC_DB_FILE_ENTRY_HPP
#define BRFC_DB_FILE_ENTRY_HPP

#include <brfc/DateTime.hpp>
#include <brfc/oh5/Oh5File.hpp>

namespace brfc {

/**
 * @brief oh5::File entry in Database
 */
class FileEntry : public oh5::Oh5File {
  public:
    virtual ~FileEntry() { }

    /**
     * @brief universally unique identifier
     */
    std::string uuid() const { return do_uuid(); }
    
    void write_to_file(const std::string& path) const {
        do_write_to_file(path);
    }

    std::string hash() const { return do_hash(); }
    
    /**
     * @brief file size in bytes
     */
    long long size() const { return do_size(); }

    DateTime stored_at() const { return do_stored_at(); }

  protected:
    virtual std::string do_uuid() const = 0;

    virtual void do_write_to_file(const std::string& path) const = 0;

    virtual std::string do_hash() const = 0;

    virtual long long do_size() const = 0;

    virtual DateTime do_stored_at() const = 0;
};

} // namespace brfc

#endif // BRFC_DB_FILE_ENTRY_HPP
