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

#include <brfc/util/BoostFileSystem.hpp>

#include <boost/filesystem.hpp>

#include <brfc/exceptions.hpp>

namespace fs = boost::filesystem;

namespace brfc {

bool
BoostFileSystem::do_exists(const std::string& path) const {
    return fs::exists(path);
}

bool
BoostFileSystem::do_is_directory(const std::string& path) const {
    return fs::is_directory(path);
}

void
BoostFileSystem::do_copy_file(const std::string& src,
                              const std::string& dst) const {
    try {
        fs::copy_file(src, dst);
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

void
BoostFileSystem::do_remove(const std::string& path) const {
    fs::remove(path);
}

bool
BoostFileSystem::do_is_absolute(const std::string& path) const {
    return fs::path(path).is_complete();
}

std::string
BoostFileSystem::do_join(const std::string& path,
                         const std::string& elem) const {
    return (fs::path(path) / fs::path(elem)).string();
}

long long
BoostFileSystem::do_file_size(const std::string& path) const {
    try {
        return fs::file_size(path);
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

void
BoostFileSystem::do_create_directory(const std::string& path) const {
    try {
        fs::create_directory(path);
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

void
BoostFileSystem::do_create_directories(const std::string& path) const {
    try {
        fs::create_directories(path);
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
}

std::vector<std::string>
BoostFileSystem::do_list_directory(const std::string& path) const {
    std::vector<std::string> result;
    try {
        fs::directory_iterator iter(path);
        for ( ; iter != fs::directory_iterator(); ++iter) {
            result.push_back(iter->filename());
        }
    } catch (const fs::filesystem_error& e) {
        throw fs_error(e.what());
    }
    return result;
}

} // namespace brfc
