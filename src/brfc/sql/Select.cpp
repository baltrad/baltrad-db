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

#include <brfc/sql/Select.hpp>

#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace sql {

Select::Select()
        : what_()
        , from_()
        , where_()
        , group_by_()
        , order_by_()
        , limit_(0)
        , offset_(0)
        , distinct_(false) {
}

void
Select::from(const Expression& table) {
    from_.from(table);
}

void
Select::join(const Expression& table,
             const Expression& condition) {
    from_.join(table, condition);
}

void
Select::outerjoin(const Expression& table,
                  const Expression& condition) {
    from_.outerjoin(table, condition);
}

void
Select::what(const Expression& expr) {
    what_.push_back(expr);
}

void
Select::append_where(const Expression& expr) {
    if (not where_.empty()) {
        where_ = Listcons().symbol("and")
                           .append(where_)
                           .append(expr)
                           .get();
    } else {
        where_ = expr;
    }
}

void
Select::append_order_by(const Expression& x, SortDirection dir) {
    Expression o;
    if (dir == ASC) {
        o.push_back(Expression::symbol("asc"));
    } else {
        o.push_back(Expression::symbol("desc"));
    }
    o.push_back(x);
    order_by_.push_back(o);
}

Expression
Select::expression() const {
    Expression stmt;
    stmt.push_back(Expression::symbol("select"));
    if (distinct_)
        stmt.push_back(Listcons().symbol("distinct").get());
    stmt.push_back(Listcons().symbol("select-columns").extend(what_).get());
    stmt.push_back(from_.expression());
    if (where_)
        stmt.push_back(Listcons().symbol("where").append(where_).get());
    if (group_by_)
        stmt.push_back(Listcons().symbol("group-by").extend(group_by_).get());
    if (order_by_)
        stmt.push_back(Listcons().symbol("order-by").extend(order_by_).get());
    if (limit_)
        stmt.push_back(Listcons().symbol("limit").int64(limit_).get());
    if (offset_)
        stmt.push_back(Listcons().symbol("offset").int64(offset_).get());
    return stmt;
}

} // namespace sql
} // namespace brfc
