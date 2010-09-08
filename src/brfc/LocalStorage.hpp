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

#ifndef BRFC_LOCAL_STORAGE_HPP
#define BRFC_LOCAL_STORAGE_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {

class FileEntry;
class String;

namespace oh5 {

class File;

} // namespace oh5

/**
 * @brief ABC for classes implementing node-local oh5::File storage
 */
class LocalStorage {
  public:
    virtual ~LocalStorage() { }

    /**
     * @brief write the (local) file to storage
     * @return oh5::File instance of the stored file
     */
    shared_ptr<const oh5::File> prestore(const FileEntry& entry) {
        return do_prestore(entry);
    }

    /**
     * @brief write the file to storage from FileEntry content stream
     * @return oh5::File instance of the stored file
     */
    shared_ptr<const oh5::File> store(const FileEntry& entry) {
        return do_store(entry);
    }
    
    /**
     * @brief remove the file if it exists
     * @return true if the file is removed (or is not stored here)
     */
    bool remove(const FileEntry& entry) {
        return do_remove(entry);
    }

  protected:
    /**
     * @return stored oh5::File instance
     */
    virtual shared_ptr<const oh5::File> do_prestore(const FileEntry& entry) = 0;

    /**
     * @return stored oh5::File instance
     */
    virtual shared_ptr<const oh5::File> do_store(const FileEntry& entry) = 0;
 
    /**
     * @return true if the file no longer exists
     */
    virtual bool do_remove(const FileEntry& entry) = 0;
};

} // namespace brfc

#endif // BRFC_LOCAL_STORAGE_HPP
