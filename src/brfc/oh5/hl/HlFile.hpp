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

#ifndef BRFC_OH5_HL_HL_FILE_H
#define BRFC_OH5_HL_HL_FILE_H

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/oh5/PhysicalOh5File.hpp>
#include <brfc/oh5/MemoryOh5NodeBackend.hpp>

namespace brfc {
namespace oh5 {
namespace hl {

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 */
class HlFile : public PhysicalOh5File {
  public:
    /**
     * @brief construct an empty File
     *
     * this is mainly useful for testing purposes
     */
    HlFile();

    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @throw fs_error if file can not be opened
     */
    explicit HlFile(const std::string& path);
    
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
    HlFile(const std::string& object,
           const Date& date,
           const Time& time,
           const std::string& source,
           const std::string& version="H5rad 2.0");

    /**
     * @brief destructor
     */
    virtual ~HlFile();

    using oh5::PhysicalOh5File::path;

    void path(const std::string& path) {
        path_ = path;
    }
     
  protected:
    /**
     * @brief absolute file path
     */
    virtual const std::string& do_path() const { return path_; }

    virtual const Oh5Node& do_root() const { return nodes_.root(); }

  private:
    void load();
    
    MemoryOh5NodeBackend nodes_;
    std::string path_;
};

} // namesapce hl
} // namespace oh5
} // namespace brfc
#endif // BRFC_OH5_HL_HL_FILE_H
