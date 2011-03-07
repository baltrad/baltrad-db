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

#include <brfc/fuse/FileEntry.hpp>

#include <cstdlib>
#include <vector>
#include <unistd.h>

#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>

#include <iostream>

namespace brfc {
namespace fuse {

FileEntry::FileEntry(db::Database* db,
                     const db::FileEntry& fe,
                     const std::string& name)
        : db_(db)
        , name_(name)
        , uuid_(fe.uuid())
        , valid_(true)
        , st_() {
    struct tm t = fe.stored_at().to_tm();
    time_t mtime = mktime(&t);
    st_.st_mode = S_IFREG | 0444;
    st_.st_nlink = 1;
    st_.st_size = fe.size();
    st_.st_atime = mtime;
    st_.st_mtime = mtime;
    st_.st_ctime = mtime;
}

FileEntry::~FileEntry() {

}

std::string
FileEntry::tmpfspath() const {
    // XXX: this relies on file being present in one directory only
    return "/tmp/bdbfs_" + uuid_;
}

int
FileEntry::do_open(fuse_file_info& fi) {
    const std::string& path = tmpfspath();
    db_->entry_by_uuid(uuid_)->write_to_file(path);
    int res = ::open(path.c_str(), O_RDONLY);
    if (res == -1)
        return -errno;
    fi.fh = res;
    return 0;
}

int
FileEntry::do_read(char* buf,
                   size_t size,
                   off_t offset,
                   fuse_file_info& fi) {
    int res = pread(fi.fh, buf, size, offset);
    if (res == -1)
        return -errno;
    return res;
}

int
FileEntry::do_close(fuse_file_info& fi) {
    int ret = ::close(fi.fh);
    return ret;
}

} // namespace fuse
} // namespace brfc
