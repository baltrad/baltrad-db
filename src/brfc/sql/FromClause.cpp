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

#include <brfc/sql/FromClause.hpp>

#include <boost/foreach.hpp>

namespace brfc {
namespace sql {

FromClause::FromClause(const Expression& table)
        : from_(table)
        , parts_() {
}

void
FromClause::from(const Expression& table) {
    from_ = table;
}

void
FromClause::join(const Expression& table,
                 const Expression& condition) {
    parts_.push_back(join_part("join", table, condition));
}

void
FromClause::outerjoin(const Expression& table,
                      const Expression& condition) {
    parts_.push_back(join_part("outerjoin", table, condition));
}

bool
FromClause::contains(const Expression& table) const {
    if (from_ == table)
        return true;
    BOOST_FOREACH(const join_part& p, parts_) {
        if (p.table == table)
            return true;
    }
    return false;
}

Expression
FromClause::expression() const {
    Expression clause;

    clause.push_back(Expression::symbol("from"));
    clause.push_back(from_);
    BOOST_FOREACH(const join_part& p, parts_) {
        Expression e;
        e.push_back(Expression::symbol(p.type));
        e.push_back(p.table);
        e.push_back(p.condition);
        clause.push_back(e);
    }

    return clause;
}

} // namespace sql
} // namespace brfc
