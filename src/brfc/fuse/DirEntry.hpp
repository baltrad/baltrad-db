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

#ifndef BRFC_FUSE_DIR_ENTRY_HPP
#define BRFC_FUSE_DIR_ENTRY_HPP

#include <errno.h>

#include <brfc/smart_ptr.hpp>
#include <brfc/fuse/Entry.hpp>

namespace brfc {
namespace fuse {

class EntryFactory;

class DirEntry : public Entry {
  public:
    /**
     * @brief constructor
     * @param name name of this directory
     * @param cfactory child factory (cloned)
     */
    explicit DirEntry(const std::string& name,
                      const EntryFactory& cfactory);

    virtual ~DirEntry();

    EntryFactory& child_factory() { return *cfactory_; }

    std::string name() const { return name_; }
    bool valid() const { return valid_; }
    void valid(bool valid) { valid_ = valid; }

  private:
    virtual int do_stat(struct stat& stbuf);

    virtual int do_readdir(void* buf,
                           fuse_fill_dir_t filler,
                           off_t offset,
                           fuse_file_info&);

    virtual int do_open(fuse_file_info& fi) {
        return -EISDIR;
    }
    
    virtual int do_read(char* buf,
                        size_t size,
                        off_t offset,
                        fuse_file_info& fi) {
        return -EISDIR;
    }
    
    virtual int do_close(fuse_file_info& fi) {
        return -EBADF;
    }

    virtual bool do_is_up_to_date() const;

    virtual void do_update();

    virtual Entry* do_child(const std::string& name);

    std::string name_;
    scoped_ptr<EntryFactory> cfactory_;
    bool valid_;
};

} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_DIR_ENTRY_HPP
