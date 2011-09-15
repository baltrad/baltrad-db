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

#ifndef BRFC_UTIL_FILE_SYSTEM_HPP
#define BRFC_UTIL_FILE_SYSTEM_HPP

#include <string>
#include <vector>

namespace brfc {

/**
 * @brief interface for accessing the file system
 */
class FileSystem {
  public:
    virtual ~FileSystem() { }

    /**
     * @brief test if @c path exists in the file system
     * @throw fs_error if the result could not be determined
     *                 (e.g access error occurs)
     */
    bool exists(const std::string& path) const {
        return do_exists(path);
    }

    /**
     * @brief test if @c path is a directory
     * @throw fs_error if the result could not be determined
     *                 (e.g access error occurs)
     */
    bool is_directory(const std::string& path) const {
        return do_is_directory(path);
    }
    
    /**
     * @brief copy a file to a new location
     * @throw fs_error if copying fails
     */
    void copy_file(const std::string& src, const std::string& dst) const {
        do_copy_file(src, dst);
    }

    /**
     * @brief remove @c path from the file system
     * @throw fs_error if removing fails
     */
    void remove(const std::string& path) const {
        do_remove(path);
    }


    /**
     * @brief is the path absolute
     */
    bool is_absolute(const std::string& path) const {
        return do_is_absolute(path);
    }
    
    /**
     * @brief add @c elem to @c path
     * @return the formed path
     */
    std::string join(const std::string& path, const std::string& elem) const {
        return do_join(path, elem);
    }
    
    /**
     * @brief get file size
     * @throw fs_error if the result could not be determined
     *                 (e.g access error occurs)
     */
    long long file_size(const std::string& path) const {
        return do_file_size(path);
    }

    /**
     * @brief create a directory
     */
    void create_directory(const std::string& path) const {
        do_create_directory(path);
    }
    
    /**
     * @brief create all missing directories on path
     */
    void create_directories(const std::string& path) const {
        do_create_directories(path);
    }

    std::vector<std::string> list_directory(const std::string& path) const {
        return do_list_directory(path);
    }
    
    /**
     * @brief remove all files in directory @c path
     */
    void clear_directory(const std::string& path) const;
  
  private:
    virtual bool do_exists(const std::string& path) const = 0;
    virtual bool do_is_directory(const std::string& path) const = 0;
    virtual void do_copy_file(const std::string& src,
                              const std::string& dst) const = 0;
    virtual void do_remove(const std::string& path) const = 0;
    virtual bool do_is_absolute(const std::string& path) const = 0;
    virtual std::string do_join(const std::string& path,
                                const std::string& elem) const = 0;
    
    virtual long long do_file_size(const std::string& path) const = 0;
    
    virtual void do_create_directory(const std::string& path) const = 0;
    virtual void do_create_directories(const std::string& path) const = 0;
    virtual std::vector<std::string> do_list_directory(const std::string& path) const = 0;
};

} // namespace brfc

#endif // BRFC_UTIL_FILE_SYSTEM_HPP
