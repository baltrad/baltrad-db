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
#include <brfc/FileEntry.hpp>

#include <brfc/oh5/File.hpp>

namespace fs = boost::filesystem;

namespace brfc {

CacheDirStorage::CacheDirStorage(const String& dir)
        : dir_(dir) {
    check_dir();
}

CacheDirStorage::~CacheDirStorage() {

}

void
CacheDirStorage::check_dir() const {
    std::string dir_utf8 = dir_.to_utf8();
    fs::path fs_path = fs::path(dir_utf8);
    try {
        if (not fs_path.is_complete())
            throw fs_error("'" + dir_utf8 + "' is not a complete path");
        if (not fs::exists(fs_path))
            throw fs_error("'" + dir_utf8 + "' does not exist");
        if (not fs::is_directory(fs_path))
            throw fs_error("'" + dir_utf8 + "' is not a directory");
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

String
CacheDirStorage::entry_path(const FileEntry& entry) const {
    return dir_ + "/" + String::number(entry.id()) + ".h5";
}

shared_ptr<const oh5::File>
CacheDirStorage::do_store(const FileEntry& entry) {
    const String& path = entry_path(entry);
    if (not fs::exists(path.to_utf8())) {
        entry.write_to_file(path);
    }
    return oh5::File::from_filesystem(path);
}

shared_ptr<const oh5::File>
CacheDirStorage::do_prestore(const FileEntry& entry) {
    const String& path = entry_path(entry);
    
    fs::copy_file(entry.file()->path().to_utf8(), path.to_utf8());
    
    //XXX: note that this is very inefficient. The content remains the same,
    //     only the path changes, so the loaded structures should be reused
    //     instead of reading the file from the filesystem again.
    return oh5::File::from_filesystem(path);
}

bool
CacheDirStorage::do_remove(const FileEntry& entry) {
    fs::path fs_path(entry_path(entry).to_utf8());

    if (fs::exists(fs_path)) {
        fs::remove(fs_path);
    }

    return not fs::exists(fs_path);
}

} // namespace brfc
