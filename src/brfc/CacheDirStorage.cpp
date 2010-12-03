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

#include <brfc/CacheDirStorage.hpp>

#include <boost/filesystem.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/File.hpp>

namespace fs = boost::filesystem;

namespace brfc {

CacheDirStorage::CacheDirStorage(const std::string& dir)
        : dir_(dir) {
    check_dir();
}

CacheDirStorage::~CacheDirStorage() {

}

void
CacheDirStorage::check_dir() const {
    fs::path fs_path = fs::path(dir_);
    try {
        if (not fs_path.is_complete())
            throw fs_error("'" + dir_ + "' is not a complete path");
        if (not fs::exists(fs_path))
            throw fs_error("'" + dir_ + "' does not exist");
        if (not fs::is_directory(fs_path))
            throw fs_error("'" + dir_ + "' is not a directory");
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

std::string
CacheDirStorage::entry_path(const db::FileEntry& entry) const {
    return dir_ + "/" + entry.uuid() + ".h5";
}

std::string
CacheDirStorage::do_store(const db::FileEntry& entry) {
    const std::string& path = entry_path(entry);
    if (not fs::exists(path)) {
        entry.write_to_file(path);
    }
    return path;
}

std::string
CacheDirStorage::do_prestore(const db::FileEntry& entry, const std::string& path) {
    const std::string& new_path = entry_path(entry);
    
    fs::copy_file(path, new_path);
    return new_path;
}

bool
CacheDirStorage::do_remove(const db::FileEntry& entry) {
    fs::path fs_path(entry_path(entry));

    if (fs::exists(fs_path)) {
        fs::remove(fs_path);
    }

    return not fs::exists(fs_path);
}

void
CacheDirStorage::do_clean() {
    fs::directory_iterator iter(dir_);
    for ( ; iter != fs::directory_iterator(); ++iter) {
        fs::remove(iter->path());
    }
}

} // namespace brfc
