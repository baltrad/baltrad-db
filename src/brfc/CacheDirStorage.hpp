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

#ifndef BRFC_CACHE_DIR_STORAGE_HPP
#define BRFC_CACHE_DIR_STORAGE_HPP

#include <boost/shared_ptr.hpp>

#include <brfc/LocalStorage.hpp>

#include <boost/thread/mutex.hpp>
#include <brfc/util/FixedSizeSet.hpp>

namespace brfc {

class FileSystem;

/**
 * @brief Storage caching files in local filesystem
 *
 * @ingroup exposed
 */
class CacheDirStorage : public LocalStorage {
  public:
    /**
     * @brief constructor
     * @param dir path in local filesystem to use as storage root
     * @param max_size size limit
     *
     * BoostFileSystem is used for filesystem access.
     */
    CacheDirStorage(const std::string& dir, size_t max_size=1000);
    
    /**
     * @brief initialize
     *
     * bring up-to-date with the filesystem
     */
    void init();
    
    /**
     * @brief destructor
     */
    virtual ~CacheDirStorage();
    
    /**
     * @brief absolute path this entry should be stored to
     */
    std::string entry_path(const FileEntry& entry) const;
    
    /**
     * @brief set FileSystem implementation
     */
    void file_system(const FileSystem* fs);
 
  private:
    virtual std::string do_store(const FileEntry& entry,
                                 const std::string& path);
    virtual std::string do_store(const FileEntry& entry);
    virtual bool do_remove(const FileEntry& entry);
  
    const FileSystem& fs() const;
    
    void remove_from_filesystem(const std::string& path);

    boost::mutex mutex_;
    std::string dir_;
    FixedSizeSet<std::string> files_;
    boost::shared_ptr<const FileSystem> fs_;
};

} // namespace brfc

#endif // BRFC_CACHE_DIR_STORAGE_HPP
