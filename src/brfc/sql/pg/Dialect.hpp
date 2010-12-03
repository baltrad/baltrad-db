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

#ifndef BRFC_SQL_PG_DIALECT_HPP
#define BRFC_SQL_PG_DIALECT_HPP

#include <brfc/sql/Dialect.hpp>
#include <string>

namespace brfc {
namespace sql {
namespace pg {

class Dialect : public sql::Dialect {
  public:
    explicit Dialect(int version=0)
            : name_("postgresql")
            , version_(version) {
    }

    void version(int version) { version_ = version; }

  protected:
    virtual bool do_has_feature(Feature feature) const;
    virtual const std::string& do_name() const;
  
  private:
    std::string name_;
    int version_;
};

} // namespace pg
} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_PG_DIALECT_HPP
