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

#include <brfc/db/AttributeQuery.hpp>

#include <utility>

#include <brfc/exceptions.hpp>
#include <brfc/expr/Listcons.hpp>

namespace brfc {

AttributeQuery::AttributeQuery()
        : distinct_()
        , fetch_()
        , filter_()
        , order_()
        , limit_(0) {
}

AttributeQuery::AttributeQuery(const AttributeQuery& other)
        : distinct_(other.distinct_)
        , fetch_(other.fetch_)
        , filter_(other.filter_)
        , order_(other.order_)
        , limit_(other.limit_) {
}

AttributeQuery::~AttributeQuery() {
    
}

AttributeQuery&
AttributeQuery::operator=(AttributeQuery rhs) {
    rhs.swap(*this);
    return *this;
}

void
AttributeQuery::swap(AttributeQuery& other) {
    std::swap(distinct_, other.distinct_);
    std::swap(fetch_, other.fetch_);
    std::swap(filter_, other.filter_);
    std::swap(order_, other.order_);
    std::swap(limit_, other.limit_);
}

AttributeQuery&
AttributeQuery::distinct(bool distinct) {
    distinct_ = distinct;
    return *this;
}

AttributeQuery&
AttributeQuery::fetch(const std::string& name, const Expression& expr) {
    if (name.empty())
        throw std::invalid_argument("empty name for AttributeQuery::fetch");
    if (fetch_.find(name) != fetch_.end())
        throw duplicate_entry("duplicate AttributeQuery::fetch: " + name);
    fetch_[name] = expr;
    return *this;
}

AttributeQuery&
AttributeQuery::filter(const Expression& expr) {
    if (not filter_.empty()) {
        filter_ = Listcons().symbol("and")
                                  .append(filter_)
                                  .append(expr)
                                  .get();
    } else {
        filter_ = expr;
    }
    return *this;
}

AttributeQuery&
AttributeQuery::group(const Expression& expr) {
    group_.push_back(expr);
    return *this;
}

AttributeQuery&
AttributeQuery::order_by(const Expression& expr, SortDir dir) {
    order_.push_back(std::make_pair(expr, dir));
    return *this;
}

} // namespace brfc
