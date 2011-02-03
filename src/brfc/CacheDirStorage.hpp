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

#ifndef BRFC_CACHE_DIR_STORAGE_HPP
#define BRFC_CACHE_DIR_STORAGE_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/LocalStorage.hpp>

namespace brfc {

class FileSystem;

/**
 * @brief Storage caching files in local filesystem
 * 
 * @note at the moment this is pretty naive
 */
class CacheDirStorage : public LocalStorage {
  public:
    /**
     * @brief constructor
     * @param dir path in local filesystem to use as storage root
     *
     * BoostFileSystem is used for filesystem access.
     */
    CacheDirStorage(const std::string& dir);
    
    /**
     * @brief constructor
     * @param dir path in local filesystem to use as storage root
     * @param fs filesystem access (caller retains ownership)
     */
    CacheDirStorage(const std::string& dir, const FileSystem* fs);

    virtual ~CacheDirStorage();
    
    /**
     * @brief absolute path this entry should be stored to
     */
    std::string entry_path(const db::FileEntry& entry) const;
 
  protected:
    virtual std::string do_prestore(const db::FileEntry& entry, const std::string& path);
    virtual std::string do_store(const db::FileEntry& entry);
    virtual bool do_remove(const db::FileEntry& entry);
    virtual void do_clean();
    virtual bool do_is_valid() const;
  
  private:
    const FileSystem& fs() const;

    std::string dir_;
    shared_ptr<const FileSystem> fs_;
};

} // namespace brfc

#endif // BRFC_CACHE_DIR_STORAGE_HPP
