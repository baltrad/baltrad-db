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

#ifndef BRFC_FUSE_ENTRY_HPP
#define BRFC_FUSE_ENTRY_HPP

#include <sys/stat.h>

#include <string>

#include <boost/noncopyable.hpp>

#include <fuse.h>

namespace brfc {
namespace fuse {

/**
 * @brief ABC for filesystem entries
 */
class Entry : public boost::noncopyable {
  public:
    virtual ~Entry() { }
    
    /**
     * @brief stat this entry
     */
    int stat(struct stat& stbuf) {
        if (not is_up_to_date())
            update();
        return do_stat(stbuf);
    }
    
    /**
     * @brief read the content of this directory
     */
    int readdir(void* buf,
                fuse_fill_dir_t filler,
                off_t offset,
                fuse_file_info& fi) {
            return do_readdir(buf, filler, offset, fi);
    }

    /**
     * @brief open file
     */
    int open(fuse_file_info& fi) {
        return do_open(fi);
    }
    
    /**
     * @brief read from previously opened file
     */
    int read(char* buf,
             size_t size,
             off_t offset,
             fuse_file_info& fi) {
        return do_read(buf, size, offset, fi);
    }
    
    /**
     * @brief close a previously opened file
     */
    int close(fuse_file_info& fi) {
        return do_close(fi);
    }

    /**
     * @brief is this entry up-to-date
     */
    bool is_up_to_date() const {
        return do_is_up_to_date();
    }
    
    /**
     * @brief bring this entry up-to-date
     */
    void update() {
        do_update();
    }
    
    /**
     * @brief find an entry by path
     * @param path absolute path to the entry
     * @return found entry or NoEntry instance if not found
     */
    Entry& find(const char* path);
    
    /**
     * @brief access child by name
     * @return pointer to the child or null pointer if not found
     */
    Entry* child(const std::string& name) {
        if (not is_up_to_date())
            update();
        return do_child(name);
    }

  private:
    virtual int do_stat(struct stat& stbuf) = 0;

    virtual int do_readdir(void* buf,
                           fuse_fill_dir_t filler,
                           off_t offset,
                           fuse_file_info&) = 0;

    virtual int do_open(fuse_file_info& fi) = 0;
    
    virtual int do_read(char* buf,
                        size_t size,
                        off_t offset,
                        fuse_file_info& fi) = 0;
    
    virtual int do_close(fuse_file_info& fi) = 0;
    
    virtual bool do_is_up_to_date() const = 0;

    virtual void do_update() = 0;

    virtual Entry* do_child(const std::string& name) = 0;

};

} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_ENTRY_HPP
