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

#include <brfc/db/FileQuery.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace db {

FileQuery::FileQuery()
        : filter_()
        , order_()
        , limit_(0)
        , skip_(0) {
}

FileQuery::FileQuery(const FileQuery& other)
        : filter_(other.filter_)
        , order_(other.order_)
        , limit_(other.limit_)
        , skip_(other.skip_) {

}

FileQuery::~FileQuery() {
    
}

FileQuery&
FileQuery::operator=(FileQuery rhs) {
    rhs.swap(*this);
    return *this;
}

void
FileQuery::swap(FileQuery& other) {
    std::swap(filter_, other.filter_);
    std::swap(order_, other.order_);
    std::swap(limit_, other.limit_);
    std::swap(skip_, other.skip_);
}

FileQuery&
FileQuery::filter(const expr::Expression& expr) {
    if (not filter_.empty()) {
        filter_ = expr::listcons().symbol("and")
                                  .append(filter_)
                                  .append(expr)
                                  .get();
    } else {
        filter_ = expr;
    }
    return *this;
}

FileQuery&
FileQuery::order_by(const expr::Expression& expr, SortDir dir) {
    order_.push_back(std::make_pair(expr, dir));
    return *this;
}

} // namespace db
} // namespace brfc
