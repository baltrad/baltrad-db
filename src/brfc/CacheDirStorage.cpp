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

#include <brfc/exceptions.hpp>

#include <brfc/db/FileEntry.hpp>

#include <brfc/oh5/File.hpp>

#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {

CacheDirStorage::CacheDirStorage(const std::string& dir)
        : dir_(dir)
        , fs_(new BoostFileSystem()) {
}

CacheDirStorage::CacheDirStorage(const std::string& dir, const FileSystem* fs)
        : dir_(dir)
        , fs_(fs, no_delete) {
}

CacheDirStorage::~CacheDirStorage() {

}

const FileSystem&
CacheDirStorage::fs() const {
    return *fs_;
}

std::string
CacheDirStorage::entry_path(const db::FileEntry& entry) const {
    return fs().join(dir_, entry.uuid() + ".h5");
}

std::string
CacheDirStorage::do_store(const db::FileEntry& entry) {
    const std::string& path = entry_path(entry);
    if (not fs().exists(path)) {
        entry.write_to_file(path);
    }
    return path;
}

std::string
CacheDirStorage::do_prestore(const db::FileEntry& entry, const std::string& path) {
    const std::string& new_path = entry_path(entry);
    fs().copy_file(path, new_path);
    return new_path;
}

bool
CacheDirStorage::do_remove(const db::FileEntry& entry) {
    const std::string& fs_path(entry_path(entry));

    if (fs().exists(fs_path)) {
        fs().remove(fs_path);
    }

    return not fs().exists(fs_path);
}

void
CacheDirStorage::do_clean() {
    fs().clear_directory(dir_);
}

} // namespace brfc
