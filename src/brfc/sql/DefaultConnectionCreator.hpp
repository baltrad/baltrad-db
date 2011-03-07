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

#ifndef BRFC_SQL_DEFAULT_CONNETION_CREATOR_HPP
#define BRFC_SQL_DEFAULT_CONNETION_CREATOR_HPP

#include <brfc/Url.hpp>
#include <brfc/sql/ConnectionCreator.hpp>

namespace brfc {
namespace sql {

/**
 * @brief default ConnectionCreator implementation
 */
class DefaultConnectionCreator : public ConnectionCreator {
  public:
    /**
     * @brief constructor
     * @param dsn database url
     */
    DefaultConnectionCreator(const Url& dsn);
    
    /**
     * @brief destructor
     */
    virtual ~DefaultConnectionCreator();
  
  protected:
    /**
     * @brief create a Connection instance from dsn
     * @throw value_error if url with invalid scheme is supplied
     *
     * construct a connection according to Url::scheme.
     *
     * schemes map as:
     *  - postgresql -> PostgresConnection
     */
    virtual Connection* do_create() const;

    virtual DefaultConnectionCreator* do_clone() const;

  private:
    Url dsn_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_DEFAULT_CONNETION_CREATOR_HPP
