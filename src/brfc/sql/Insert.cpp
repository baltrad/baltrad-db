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

#include <brfc/sql/Insert.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/expr/Listcons.hpp>

#include <brfc/sql/Factory.hpp>

namespace brfc {
namespace sql {

Insert::Insert(const std::string& table)
        : table_(table)
        , values_()
        , returning_() {
}

void
Insert::value(const std::string& column, const Expression& expr) {
    values_[column] = expr;
}

void
Insert::returning(const Expression& expr) {
    returning_ = expr;
}

Expression
Insert::expression() const {
    Factory xpr;
    Expression stmt;
    stmt.push_back(Expression::symbol("insert"));
    stmt.push_back(xpr.table(table_));

    Expression cols, vals;
    cols.push_back(Expression::symbol("insert-columns"));
    vals.push_back(Expression::symbol("insert-values"));
    BOOST_FOREACH(const ValueMap::value_type& e, values_) {
        cols.push_back(Expression(e.first));
        vals.push_back(e.second);
    }

    stmt.push_back(cols);
    stmt.push_back(vals);
    
    if (returning_) {
        stmt.push_back(Listcons().symbol("returning").append(returning_).get());
    }
    return stmt;
}

} // namespace sql
} // namespace brfc
