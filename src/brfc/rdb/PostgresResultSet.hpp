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

#ifndef BRFC_RDB_POSTGRES_RESULT_SET_HPP
#define BRFC_RDB_POSTGRES_RESULT_SET_HPP

#include <brfc/ResultSet.hpp>

#include <pqxx/result>

namespace brfc {
namespace rdb {

class PgTypes;

class PostgresResultSet : public ResultSet {
  public:
    PostgresResultSet(const pqxx::result& result, const PgTypes* types);
  
  protected:
    virtual bool do_next();
    virtual bool do_seek(int idx);
    virtual int do_size();
    
    /**
     * @brief get value at @c pos
     * @return NULL Variant if value at pos is NULL, string Variant otherwise
     * @throw lookup_error if pos is out of rance
     */
    virtual Variant do_value_at(unsigned int pos) const;

  private:
    pqxx::result result_;
    const PgTypes* types_;
    int row_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_POSTGRES_RESULT_SET_HPP
