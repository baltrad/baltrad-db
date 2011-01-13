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

#include <brfc/db/AttributeQuery.hpp>

#include <utility>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/AttributeResult.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Function.hpp>

namespace brfc {
namespace db {

AttributeQuery::AttributeQuery(Database* db)
        : db_(db)
        , distinct_()
        , fetch_()
        , filter_()
        , order_()
        , limit_(0) {
}

AttributeQuery::AttributeQuery(const AttributeQuery& other)
        : db_(other.db_)
        , distinct_(other.distinct_)
        , fetch_(other.fetch_)
        , filter_(other.filter_)
        , order_(other.order_)
        , limit_(other.limit_) {

}

AttributeQuery::~AttributeQuery() {
    
}

AttributeQuery&
AttributeQuery::distinct(bool distinct) {
    distinct_ = true;
    return *this;
}

AttributeQuery&
AttributeQuery::fetch(const expr::Attribute& attribute) {
    fetch_.push_back(attribute.clone());
    // XXX: there used to be duplicate check here, removed due to
    //      adding fetch(Function). This should be brought back
    //      and fetch(...) made to accept labeled expressions only.
    return *this;
}

AttributeQuery&
AttributeQuery::fetch(const expr::Function& function) {
    fetch_.push_back(function.clone());
    return *this;
}

AttributeQuery&
AttributeQuery::filter(const expr::Expression& expr) {
    filter_ = filter_ ? filter_->and_(expr) : expr.clone();
    return *this;
}

AttributeQuery&
AttributeQuery::group(const expr::Expression& expr) {
    group_.push_back(expr.clone());
    return *this;
}

AttributeQuery&
AttributeQuery::order_by(const expr::Expression& expr, SortDir dir) {
    order_.push_back(std::make_pair(expr.clone(), dir));
    return *this;
}

AttributeResult*
AttributeQuery::execute() {
    if (not db_)
        throw std::runtime_error("AttributeQuery not associated with Database");
    return db_->execute(*this);
}

} // namespace db
} // namespace brfc
