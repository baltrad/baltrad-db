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

#include <brfc/String.hpp>

namespace brfc {

class FileEntry;

/**
 * @brief ABC for classes implementing node-local oh5::File storage
 */
class LocalStorage {
  public:
    virtual ~LocalStorage() { }

    /**
     * @brief write the (local) file to storage
     * @param entry fileentry the file was associated with
     * @param path absolute path to the file in filesystem
     * @return absolute path to the stored file
     */
    String prestore(const FileEntry& entry, const String& path) {
        return do_prestore(entry, path);
    }

    /**
     * @brief write the file to storage from FileEntry content stream
     * @return absolute path to the stored file
     */

    String store(const FileEntry& entry) {
        return do_store(entry);
    }
    
    /**
     * @brief remove the file if it exists
     * @return true if the file is removed (or is not stored here)
     */
    bool remove(const FileEntry& entry) {
        return do_remove(entry);
    }
    
    /**
     * @brief remove all files
     */
    void clean() {
        do_clean();
    }

  protected:
    /**
     * @return stored oh5::File instance
     */
    virtual String do_prestore(const FileEntry& entry, const String& path) = 0;

    /**
     * @return stored oh5::File instance
     */
    virtual String do_store(const FileEntry& entry) = 0;
 
    /**
     * @return true if the file no longer exists
     */
    virtual bool do_remove(const FileEntry& entry) = 0;

    virtual void do_clean() = 0;
};

} // namespace brfc

#endif // BRFC_LOCAL_STORAGE_HPP
