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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_RDB_SELECT_HPP
#define BRFC_RDB_SELECT_HPP

#include <brfc/rdb/Selectable.hpp>
#include <vector>

namespace brfc {
namespace rdb {

class Select : public Selectable {
  public:
    virtual QString name() const { return ""; }

    static SelectPtr create() {
        return SelectPtr(new Select());
    }

    void what(expr::ExpressionPtr expr) {
        what_.push_back(expr);
    }

    void from(FromClausePtr from) { from_ = from; }
    
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
     * @brief append Expression to where clause
     */
    void append_where(expr::ExpressionPtr expr);

    /**
     * @brief set Expression as where clause
     */
    void where(expr::ExpressionPtr expr) {
        where_ = expr;
    }

    const std::vector<expr::ExpressionPtr>& what() const {
        return what_;
    }

    FromClausePtr from() const {
        return from_;
    }


    expr::ExpressionPtr where() const {
        return where_;
    }

    std::vector<expr::ExpressionPtr>& what() {
        return what_;
    }

    FromClausePtr from() {
        return from_;
    }


    expr::ExpressionPtr where() {
        return where_;
    }

  
  protected:
    Select();

  private:
    std::vector<expr::ExpressionPtr> what_;
    FromClausePtr from_;
    expr::ExpressionPtr where_;
    bool distinct_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SELECT_HPP
