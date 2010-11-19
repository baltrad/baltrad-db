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

#ifndef BRFC_SQL_LARGE_OBJECT_PROXY_HPP
#define BRFC_SQL_LARGE_OBJECT_PROXY_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/sql/LargeObject.hpp>

namespace brfc {
namespace sql {

class Connection;

/**
 * @brief proxy for LargeObject
 */
class LargeObjectProxy : public LargeObject {
  public:
    /**
     * @brief constructor
     * @param conn the database connection
     * @param lobj the proxied large object
     */
    LargeObjectProxy(shared_ptr<Connection> conn,
                     shared_ptr<LargeObject> lobj);
    
    virtual ~LargeObjectProxy();

  protected:
    virtual long long do_id() const;
    virtual void do_write_to_file(const String& path) const;
  
  private:
    shared_ptr<Connection> conn_;
    shared_ptr<LargeObject> lobj_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_LARGE_OBJECT_PROXY_HPP
