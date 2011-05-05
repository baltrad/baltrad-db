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

#ifndef BRFC_SQL_SELECT_HPP
#define BRFC_SQL_SELECT_HPP

#include <brfc/expr/Expression.hpp>
#include <brfc/sql/FromClause.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL SELECT statement
 *
 * @pre
 * (select [(distinct)]
 *         (select-columns expr ...)
 *         (from ...)
 *         (where expr)
 *         [(group-by expr ...)]
 *         [(order-by (asc expr) (desc expr) ...)]
 *         [(limit l)]
 *         [(offset o)])
 */
class Select {
  public:
    enum SortDirection {
        ASC = 1,
        DESC = 2
    };

    Select();

    /**
     * @brief want unique results
     */
    void distinct(bool distinct) {
        distinct_ = distinct;
    }
    
    /**
     * @brief is distinct set?
     */
    bool distinct() const {
        return distinct_;
    }
    
    /**
     * @throw duplicate_entry if the @c expr is labeled and the name
     *        conflicts with a previous label or column
     */
    void what(const expr::Expression& expr);

    /**
     * @brief append Expression to where clause
     */
    void append_where(const expr::Expression& x);

    /**
     * @brief set Expression as where clause
     */
    void where(const expr::Expression& expr) {
        where_ = expr;
    }

    expr::Expression where() const {
        return where_;
    }

    expr::Expression what() const {
        return what_;
    }

    void from(const FromClause& from) { from_ = from; }

    const FromClause& from() const {
        return from_;
    }

    void from(const expr::Expression& table);

    void join(const expr::Expression& table,
              const expr::Expression& condition);

    void outerjoin(const expr::Expression& table,
                   const expr::Expression& condition);

    void append_group_by(const expr::Expression& expr) {
        group_by_.push_back(expr);
    }

    expr::Expression group_by() const {
        return group_by_;
    }

    void append_order_by(const expr::Expression& x, SortDirection dir);

    expr::Expression order() const { return order_by_; }
    
    void limit(int limit) { limit_ = limit; }

    int limit() const { return limit_; }

    void offset(int offset) { offset_ = offset; }

    int offset() const { return offset_; }

    expr::Expression expression() const;

  private:
    expr::Expression what_;
    FromClause from_;
    expr::Expression where_;
    expr::Expression group_by_;
    expr::Expression order_by_;
    int limit_;
    int offset_;
    bool distinct_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_SELECT_HPP
