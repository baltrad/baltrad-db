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

#ifndef BRFC_UTIL_NAMED_TEMPORARY_FILE_HPP
#define BRFC_UTIL_NAMED_TEMPORARY_FILE_HPP

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace brfc {

class NamedTemporaryFile : public boost::noncopyable {
  public:
    /**
     * @brief constructor
     */
    NamedTemporaryFile();
    
    /**
     * @brief destructor
     *
     * if not already removed, removes the file
     */
    ~NamedTemporaryFile();
    
    /**
     * @brief remove file
     */
    void remove();
    
    /**
     * @brief absolute pathname of this file
     * @return the path or empty string if removed
     */
    std::string path();

  private:
    boost::shared_ptr<char> path_;
};

} // namespace brfc

#endif // BRFC_UTIL_NAMED_TEMPORARY_FILE_HPP
