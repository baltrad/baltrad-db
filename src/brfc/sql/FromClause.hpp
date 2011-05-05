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
    explicit FromClause(const expr::Expression& e=expr::Expression());
    
    void from(const expr::Expression& table);
    
    void join(const expr::Expression& table,
              const expr::Expression& condition);
    
    void outerjoin(const expr::Expression& table,
                   const expr::Expression& condition);

    bool contains(const expr::Expression& table) const;
 
    expr::Expression expression() const;

  private:
    struct join_part {
        join_part(const std::string& type_,
                  const expr::Expression& table_,
                  const expr::Expression& condition_)
                : type(type_)
                , table(table_)
                , condition(condition_) {
        }

        std::string type;
        expr::Expression table;
        expr::Expression condition;
    };
    
    expr::Expression from_;
    std::vector<join_part> parts_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_FROM_CLAUSE_HPP
