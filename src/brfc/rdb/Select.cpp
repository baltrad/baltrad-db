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

#include <brfc/rdb/Select.hpp>

#include <brfc/expr/Expression.hpp>
#include <brfc/expr/BinaryOperator.hpp>

#include <brfc/rdb/FromClause.hpp>

namespace brfc {
namespace rdb {

Select::Select()
        : what_()
        , from_(FromClause::create())
        , where_()
        , distinct_(false) {
}


void
Select::append_where(expr::ExpressionPtr expr) {
    where_ = where_ ? where_->and_(expr) : expr;
}

} // namespace rdb
} // namespace brfc
