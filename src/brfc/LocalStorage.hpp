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

#ifndef BRFC_LOCAL_STORAGE_HPP
#define BRFC_LOCAL_STORAGE_HPP

#include <string>

namespace brfc {

class FileEntry;

/**
 * @brief ABC for classes implementing node-local PhysicalFile storage
 * @ingroup exposed
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
    std::string prestore(const FileEntry& entry, const std::string& path) {
        return do_prestore(entry, path);
    }

    /**
     * @brief write the file to storage from FileEntry content stream
     * @return absolute path to the stored file
     */

    std::string store(const FileEntry& entry) {
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

    /**
     * @brief check if the storage is valid
     */
    bool is_valid() const {
        return do_is_valid();
    }

  protected:
    /**
     * @return absolute path to the stored file
     */
    virtual std::string do_prestore(const FileEntry& entry, const std::string& path) = 0;

    /**
     * @return absolute path to the stored file
     */
    virtual std::string do_store(const FileEntry& entry) = 0;
 
    /**
     * @return true if the file no longer exists
     */
    virtual bool do_remove(const FileEntry& entry) = 0;

    virtual void do_clean() = 0;

    virtual bool do_is_valid() const = 0;
};

} // namespace brfc

#endif // BRFC_LOCAL_STORAGE_HPP
