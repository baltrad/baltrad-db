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

#include <brfc/CacheDirStorage.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <brfc/db/FileEntry.hpp>

#include <brfc/util/no_delete.hpp>
#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {

CacheDirStorage::CacheDirStorage(const std::string& dir, size_t max_size)
        : mutex_()
        , dir_(dir)
        , files_(max_size)
        , fs_(new BoostFileSystem()) {
    files_.removal_callback(
        boost::bind(&CacheDirStorage::remove_from_filesystem, this, _1)
    );
}

CacheDirStorage::~CacheDirStorage() {

}

void
CacheDirStorage::init() {
    BOOST_FOREACH(const std::string& filename, fs().list_directory(dir_)) {
        files_.add(fs().join(dir_, filename));
    }
}

const FileSystem&
CacheDirStorage::fs() const {
    return *fs_;
}

void
CacheDirStorage::file_system(const FileSystem* fs) {
    fs_.reset(fs, no_delete);
}

std::string
CacheDirStorage::entry_path(const FileEntry& entry) const {
    return fs().join(dir_, entry.uuid() + ".h5");
}

std::string
CacheDirStorage::do_store(const FileEntry& entry) {
    boost::lock_guard<boost::mutex> lock(mutex_);

    const std::string& path = entry_path(entry);
    if (not fs().exists(path)) {
        entry.write_to_file(path);
    }
    files_.add(path);

    return path;
}

std::string
CacheDirStorage::do_store(const FileEntry& entry, const std::string& path) {
    boost::lock_guard<boost::mutex> lock(mutex_);

    const std::string& new_path = entry_path(entry);
    fs().copy_file(path, new_path);
    files_.add(new_path);

    return new_path;
}

void
CacheDirStorage::remove_from_filesystem(const std::string& path) {
    fs().remove(path);
}

bool
CacheDirStorage::do_remove(const FileEntry& entry) {
    boost::lock_guard<boost::mutex> lock(mutex_);

    const std::string& fs_path(entry_path(entry));
    files_.remove(fs_path);

    return not fs().exists(fs_path);
}

} // namespace brfc
