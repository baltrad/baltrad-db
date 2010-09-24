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

#include <brfc/LocalStorage.hpp>
#include <brfc/String.hpp>

namespace brfc {

/**
 * @brief Storage caching files in local filesystem
 * 
 * @note at the moment this is pretty naive
 */
class CacheDirStorage : public LocalStorage {
  public:
    /**
     * @param dir path in local filesystem to use as storage root
     * @param namer namer used to name new files
     */
    CacheDirStorage(const String& dir);

    virtual ~CacheDirStorage();
    
    /**
     * @brief absolute path this entry should be stored to
     */
    String entry_path(const FileEntry& entry) const;
 
  protected:
    virtual String do_prestore(const FileEntry& entry, const String& path);
    virtual String do_store(const FileEntry& entry);
    virtual bool do_remove(const FileEntry& entry);
    virtual void do_clean();
  
  private:
    void check_dir() const;

    String dir_;
};

} // namespace brfc

#endif // BRFC_CACHE_DIR_STORAGE_HPP
