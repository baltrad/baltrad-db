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

#ifndef BRFC_OH5_FILE_HPP
#define BRFC_OH5_FILE_HPP

#include <string>

#include <boost/noncopyable.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>

namespace brfc {

class Oh5Metadata;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 *
 * @ingroup exposed
 */
class Oh5File : public boost::noncopyable {
  public:
    /**
     * @brief construct an empty File
     */
    Oh5File();

    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @throw fs_error if file can not be opened
     */
    explicit Oh5File(const std::string& path);
    
    /**
     * @brief construct with mandatory attributes present
     * @param object /what/object
     * @param date /what/date
     * @param time /what/time
     * @param source /what/source
     * @param version /what/version
     *
     * this is the minimal "correct" file, given that parameters are
     * correctly formed.
     */
    Oh5File(const std::string& object,
            const Date& date,
            const Time& time,
            const std::string& source,
            const std::string& version="H5rad 2.0");

    /**
     * @brief destructor
     */
    ~Oh5File();

    const Oh5Metadata& metadata() const { return metadata_; }
    Oh5Metadata& metadata() { return metadata_; }

    /**
     * @brief absolute file path
     */
    std::string path() const {
        return path_;
    }

    void path(const std::string& value) {
        path_ = value;
    }

    std::string name() const;
    
  private:
    Oh5Metadata metadata_;
    std::string path_;
};

} // namespace brfc
#endif // BRFC_OH5_FILE_HPP
