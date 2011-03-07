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

#include <brfc/fuse/DirEntry.hpp>

#include <boost/foreach.hpp>

#include <brfc/fuse/EntryFactory.hpp>

namespace brfc {
namespace fuse {

DirEntry::DirEntry(const std::string& name,
                   const EntryFactory& cfactory)
        : name_(name)
        , cfactory_(cfactory.clone())
        , valid_(true) {
}

DirEntry::~DirEntry() {

}

int
DirEntry::do_stat(struct stat& stbuf) {
    stbuf.st_mode = S_IFDIR | 0555;
    stbuf.st_size = 4096;
    stbuf.st_nlink = 2 + cfactory_->entry_names().size();
    return 0;
}

int
DirEntry::do_readdir(void* buf,
                  fuse_fill_dir_t filler,
                  off_t offset,
                  fuse_file_info&) {
    filler(buf, ".", 0, offset); 
    filler(buf, "..", 0, offset); 
    BOOST_FOREACH(const std::string& name, cfactory_->entry_names()) {
        filler(buf, name.c_str(), 0, offset);
    }
    return 0;
}

bool
DirEntry::do_is_up_to_date() const {
    return false;
}

void
DirEntry::do_update() {
    cfactory_->update();
}

Entry*
DirEntry::do_child(const std::string& name) {
    return cfactory_->entry_by_name(name);
}

} // namespace fuse
} // namespace brfc
