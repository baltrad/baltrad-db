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

#ifndef BRFC_SQL_BIND_HPP
#define BRFC_SQL_BIND_HPP

#include <string>

#include <brfc/sql/Expression.hpp>

namespace brfc {
namespace sql {

/**
 * @brief a placeholder in sql statement
 */
class Bind : public Expression {
  public:
    static BindPtr create(const std::string& name) {
        return BindPtr(new Bind(name));
    }

    virtual const std::string& name() const { return name_; }
  
  protected:
    explicit Bind(const std::string& name)
            : name_(name) {
    }
  
  private:
    std::string name_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_BIND_HPP
