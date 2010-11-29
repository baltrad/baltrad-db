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

#ifndef BRFC_SQL_CONNETION_CREATOR_HPP
#define BRFC_SQL_CONNETION_CREATOR_HPP

#include <boost/noncopyable.hpp>

namespace brfc {
namespace sql {

class Connection;

/**
 * @brief ABC for creating Connection instances
 */
class ConnectionCreator : boost::noncopyable {
  public:
    virtual ~ConnectionCreator() { }
    
    /**
     * @brief create a Connection instance
     */
    Connection* create() const {
        return do_create();
    }
 
  protected:
    /**
     * @brief create() implementation
     */
    virtual Connection* do_create() const = 0;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_CONNETION_CREATOR_HPP