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

#ifndef BRFC_SQL_CONNECTION_POOL_HPP
#define BRFC_SQL_CONNECTION_POOL_HPP

#include <brfc/smart_ptr.hpp>

namespace brfc {
namespace sql {

class Connection;

/**
 * @brief ABC for a pool of database connections
 */
class ConnectionPool {
  public:
    /**
     * @brief destructor
     */
    virtual ~ConnectionPool() { }

    /**
     * @brief get a free database connection
     */
    shared_ptr<Connection> get() {
        return do_get();
    }
    
    /**
     * @brief return a database connection back to the pool
     */
    void put(Connection* db) {
        do_put(db);
    }

  private:
    /**
     * @brief get() implementation
     */
    virtual shared_ptr<Connection> do_get() = 0;
    
    /**
     * @brief put() implementation
     */
    virtual void do_put(Connection* db) = 0;
};

} // namespace sql
} // namepsace brfc

#endif // BRFC_SQL_CONNECTION_POOL_HPP
