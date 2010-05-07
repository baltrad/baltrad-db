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

#include <brfc/Path.hpp>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <brfc/exceptions.hpp>

namespace brfc {

String Path::sep_("/");

Path::Path(const String& path)
        : path_(path) {

}

bool
Path::exists() const {
    struct stat st;
    if (lstat(path_.to_utf8().c_str(), &st) == 0) {
        return true;
    } else if (errno == ENOENT or errno == ENOTDIR) {
        return false;
    } else {
        throw fs_error(strerror(errno) + path_.to_std_string());
    }
}

bool
Path::is_absolute() const {
    return path_.starts_with(sep_);
}

bool
Path::is_dir() const {
    struct stat st;
    if (lstat(path_.to_utf8().c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    } else if (errno == ENOENT or errno == ENOTDIR) {
        return false;
    } else {
        throw fs_error(strerror(errno) + path_.to_std_string());
    }
}

Path
Path::join(const String& path) const {
    return Path(path_ + sep_ + path);
}

} // namespace brfc
