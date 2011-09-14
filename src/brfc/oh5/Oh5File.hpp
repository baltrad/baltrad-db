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
     * @brief destructor
     */
    virtual ~Oh5File() = 0;

    const Oh5Metadata& metadata() const;
    Oh5Metadata& metadata();
    
  private:
    virtual const Oh5Metadata& do_metadata() const = 0;
};

inline Oh5File::~Oh5File() { }

} // namespace brfc
#endif // BRFC_OH5_FILE_HPP
