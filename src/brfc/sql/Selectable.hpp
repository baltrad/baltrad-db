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

#include <brfc/String.hpp>

#include <brfc/sql/fwd.hpp>
#include <brfc/sql/Element.hpp>

namespace brfc {
namespace sql {

class Selectable : public Element {
  public:
    SelectablePtr shared_from_this() const {
        return static_pointer_cast<Selectable>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }

    virtual String name() const = 0;

    virtual ColumnPtr column(const String& name) const;

    virtual std::vector<ColumnPtr> columns() const = 0;

    virtual std::vector<ColumnPtr> fk_columns() const;

    virtual ColumnPtr matching_column(const Column& col) const;

    AliasPtr alias(const String& name);

    JoinPtr join(SelectablePtr rhs, ExpressionPtr condition);

    JoinPtr outerjoin(SelectablePtr rhs, ExpressionPtr condition);

    JoinPtr crossjoin(SelectablePtr rhs);
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_SELECTABLE_HPP
