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

#ifndef BRFC_SQL_SELECTABLE_HPP
#define BRFC_SQL_SELECTABLE_HPP

#include <vector>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Element.hpp>

namespace brfc {

namespace sql {

/**
 * @brief ABC for Selectable SQL elements
 */
class Selectable : public Element {
  public:
    SelectablePtr shared_from_this() const {
        return static_pointer_cast<Selectable>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }
    
    /**
     * @brief name of this selectable
     * @return empty string
     *
     * by default, selectables are unnamed
     */
    virtual std::string name() const;
    
    /**
     * @brief access column by @c name
     */
    virtual ColumnPtr column(const std::string& name) const;
    
    /**
     * @brief columns defined on this selectable
     */
    virtual std::vector<ColumnPtr> columns() const = 0;
    
    /**
     * @brief foreign key columns defined on this selectable
     */
    virtual std::vector<ColumnPtr> fk_columns() const;
    
    /**
     * @brief find a matching column
     * @return pointer to Column or null if not found
     *
     * find a column that is either the same column or a proxy for
     * the column.
     */
    virtual ColumnPtr matching_column(const Column& col) const;
    
    /**
     * @brief alias this selectable
     */
    AliasPtr alias(const std::string& name);
    
    /**
     * @brief create an inner join from this selectable to @c rhs
     * @throw lookup_error if condition is missing and can not be
     *        automatically determined.
     */
    JoinPtr join(SelectablePtr rhs,
                 ExpressionPtr condition=ExpressionPtr());

    /**
     * @brief create a left outer join from this selectable to @c rhs
     * @throw lookup_error if condition is missing and can not be
     *        automatically determined.
     */
    JoinPtr outerjoin(SelectablePtr rhs,
                      ExpressionPtr condition=ExpressionPtr());

    /**
     * @brief create a cross join from this selectable to @c rhs
     */
    JoinPtr crossjoin(SelectablePtr rhs);
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_SELECTABLE_HPP
