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

#ifndef BRFC_SQL_PG_RESULT_HPP
#define BRFC_SQL_PG_RESULT_HPP

#include <brfc/sql/Result.hpp>

#include <pqxx/result>

namespace brfc {
namespace sql {
namespace pg {

class Types;

class Result : public sql::Result {
  public:
    Result(const pqxx::result& result, const Types* types);
  
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

    virtual Variant do_value_at(const String& column) const;

    virtual int do_affected_rows() const;

  private:
    Variant pqtype_to_variant(const pqxx::result::field& field,
                              pqxx::oid coltype) const;

    pqxx::result result_;
    const Types* types_;
    int row_;
};

} // namespace pg
} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_PG_RESULT_SET_HPP
