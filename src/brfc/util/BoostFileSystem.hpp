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

#ifndef BRFC_UTIL_BOOST_FILE_SYSTEM_HPP
#define BRFC_UTIL_BOOST_FILE_SYSTEM_HPP

#include <brfc/util/FileSystem.hpp>

namespace brfc {

/**
 * @brief access filesystem using boost::filesystem
 */
class BoostFileSystem : public FileSystem {
  private:
    virtual bool do_exists(const std::string& path) const;
    virtual bool do_is_directory(const std::string& path) const;
    virtual void do_copy_file(const std::string& src,
                              const std::string& dst) const;
    virtual void do_remove(const std::string& path) const;
    virtual bool do_is_absolute(const std::string& path) const;
    virtual std::string do_join(const std::string& path,
                                const std::string& elem) const;
    
    virtual long long do_file_size(const std::string& path) const;

    virtual void do_clear_directory(const std::string& path) const;
};

} // namespace brfc

#endif // BRFC_UTIL_BOOST_FILE_SYSTEM_HPP
