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

#include <brfc/db/FileQuery.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/FileResult.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace db {

FileQuery::FileQuery(Database* db)
        : db_(db)
        , filter_()
        , order_()
        , limit_(0)
        , skip_(0) {
}

FileQuery::FileQuery(const FileQuery& other)
        : db_(other.db_)
        , filter_()
        , order_()
        , limit_(other.limit_)
        , skip_(other.skip_) {
    if (other.filter_)
        filter_ = other.filter_->clone();
    BOOST_FOREACH(const OrderPair& opair, other.order_) {
        order_.push_back(std::make_pair(opair.first->clone(), opair.second));
    }
}

FileQuery::~FileQuery() {
    
}

FileQuery&
FileQuery::operator=(const FileQuery& rhs) {
    if (this == &rhs)
        return *this;

    db_ = rhs.db_;
    if (rhs.filter_) {
        filter_ = rhs.filter_->clone();
    } else {
        filter_.reset();
    }
    order_.clear();
    BOOST_FOREACH(const OrderPair& opair, rhs.order_) {
        order_.push_back(std::make_pair(opair.first->clone(), opair.second));
    }
    limit_ = rhs.limit_;
    skip_ = rhs.skip_;
    return *this;
}

FileQuery&
FileQuery::filter(const expr::Expression& expr) {
    filter_ = filter_ ? filter_->and_(expr) : expr.clone();
    return *this;
}

FileResult*
FileQuery::execute() {
    if (not db_)
        throw std::runtime_error("FileQuery not associated with Database");
    return db_->execute(*this);
}

FileQuery&
FileQuery::order_by(const expr::Expression& expr, SortDir dir) {
    order_.push_back(std::make_pair(expr.clone(), dir));
    return *this;
}

} // namespace db
} // namespace brfc
