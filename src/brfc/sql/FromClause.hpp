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

#ifndef BRFC_SQL_FROM_CLAUSE_HPP
#define BRFC_SQL_FROM_CLAUSE_HPP

#include <string>
#include <vector>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL JOIN between two Selectables
 *
 * (from (begin-from) table (join ...) ...)
 */
class FromClause {
  public:
    explicit FromClause(const Expression& e=Expression());
    
    void from(const Expression& table);
    
    void join(const Expression& table,
              const Expression& condition);
    
    void outerjoin(const Expression& table,
                   const Expression& condition);

    bool contains(const Expression& table) const;
 
    Expression expression() const;

  private:
    struct join_part {
        join_part(const std::string& type_,
                  const Expression& table_,
                  const Expression& condition_)
                : type(type_)
                , table(table_)
                , condition(condition_) {
        }

        std::string type;
        Expression table;
        Expression condition;
    };
    
    Expression from_;
    std::vector<join_part> parts_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_FROM_CLAUSE_HPP
