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

#ifndef BRFC_SQL_JOIN_HPP
#define BRFC_SQL_JOIN_HPP

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Selectable.hpp>

namespace brfc {
namespace sql {

/**
 * @brief SQL JOIN between two Selectables
 */
class Join : public Selectable {
  public:
    /**
     * @brief type of join
     */
    enum Type {
        CROSS = 0, ///< cross join
        INNER = 1, ///< inner join
        LEFT = 2 ///< left outer join
    };
    
    /**
     * @brief construct as a shared_ptr
     * @sa Join()
     */
    static JoinPtr create(SelectablePtr lhs,
                          SelectablePtr rhs,
                          ExpressionPtr condition=ExpressionPtr(),
                          Type type=CROSS) {
        return JoinPtr(new Join(lhs, rhs, condition, type));
    }

    /**
     * @brief left hand selectable
     */
    SelectablePtr lhs() const {
        return lhs_;
    }
    
    /**
     * @brief right hand selectable
     */
    SelectablePtr rhs() const {
        return rhs_;
    }
    
    /**
     * @brief join condition
     */
    ExpressionPtr condition() const {
        return condition_;
    }

    Type type() const { return type_; }

    /**
     * @brief test if join contains a selectable element
     *
     * if this join is part of a chain (lhs or rhs is also a join),
     * the check is made in the entire chain.
     *
     * Selectable::name is used to determine equality.
     */
    bool contains(SelectablePtr element) const;
    
    /**
     * @brief union of columns defined on left and right hand selectables
     */
    virtual std::vector<ColumnPtr> columns() const;
    
    /**
     * @brief find a join condition between two Selectables
     * @throw lookup_error if a relation can't be determined
     *
     * join condition is determined from foreign key relations between
     * the two Selectables
     */
    static ExpressionPtr find_condition(const Selectable& lhs,
                                        const Selectable& rhs);

  protected:
    /**
     * @brief constructor
     * @param lhs left hand selectable
     * @param rhs right hand selectable
     * @param condition condition to join on
     * @param type type of the join
     *
     * if the join condition is missing and the type is not CROSS, the join
     * condition is looked up using find_condition()
     */
    Join(SelectablePtr lhs,
         SelectablePtr rhs,
         ExpressionPtr condition=ExpressionPtr(),
         Type type=CROSS);

  private:
    SelectablePtr lhs_;
    SelectablePtr rhs_;
    ExpressionPtr condition_;
    Type type_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_JOIN_HPP
