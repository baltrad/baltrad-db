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

#ifndef BRFC_SQL_SELECT_HPP
#define BRFC_SQL_SELECT_HPP

#include <brfc/sql/Selectable.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL SELECT statement
 */
class Select : public Selectable {
  public:
    enum SortDirection {
        ASC = 1,
        DESC = 2
    };

    typedef std::pair<ExpressionPtr, SortDirection> OrderPair;
    typedef std::vector<OrderPair> OrderVector;

    /**
     * @brief construct as shared_ptr
     * @sa Select()
     */
    static SelectPtr create(SelectablePtr from=SelectablePtr()) {
        return SelectPtr(new Select(from));
    }

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
    
    virtual std::vector<ColumnPtr> columns() const;
 
    /**
     * @throw duplicate_entry if the @c expr is labeled and the name
     *        conflicts with a previous label or column
     */
    void what(ExpressionPtr expr);

    std::vector<ExpressionPtr>& what() {
        return what_;
    }
      
    /**
     * @brief append Expression to where clause
     */
    void append_where(ExpressionPtr expr);

    /**
     * @brief set Expression as where clause
     */
    void where(ExpressionPtr expr) {
        where_ = expr;
    }

    ExpressionPtr where() const {
        return where_;
    }

    const std::vector<ExpressionPtr>& what() const {
        return what_;
    }

    void from(SelectablePtr from) { from_ = from; }
    
    SelectablePtr from() const {
        return from_;
    }

    void append_group_by(ExpressionPtr expr) {
        group_by_.push_back(expr);
    }

    std::vector<ExpressionPtr> group_by() const {
        return group_by_;
    }

    void append_order_by(ExpressionPtr expr, SortDirection dir);

    OrderVector order() const { return order_; }
    
    void limit(int limit) { limit_ = limit; }

    int limit() const { return limit_; }

  protected:
    /**
     * @brief constructor
     * @param from a Selectable to select from. The columns of the
     *        selectable are added as columns to select and the
     *        selectable is set as the from clause.
     */

    Select(SelectablePtr from=SelectablePtr());

  private:
    std::vector<ExpressionPtr> what_;
    SelectablePtr from_;
    ExpressionPtr where_;
    std::vector<ExpressionPtr> group_by_;
    OrderVector order_;
    int limit_;
    bool distinct_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_SELECT_HPP
