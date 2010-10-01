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

#include <brfc/db/Query.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/ResultSet.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace db {

Query::Query(Database* db)
        : db_(db)
        , distinct_()
        , fetch_()
        , filter_() {
}

Query::Query(const Query& other)
        : db_(other.db_)
        , distinct_(other.distinct_)
        , fetch_(other.fetch_)
        , filter_(other.filter_) {

}

Query::~Query() {
    
}

Query&
Query::distinct(bool distinct) {
    distinct_ = true;
    return *this;
}

Query&
Query::fetch(expr::AttributePtr attribute) {
    BOOST_FOREACH(expr::AttributePtr attr, fetch_) {
        if (attr->name() == attribute->name())
            throw duplicate_entry(attribute->name().to_std_string());
    }
    fetch_.push_back(attribute);
    return *this;
}

Query&
Query::filter(expr::ExpressionPtr expr) {
    filter_ = filter_ ? filter_->and_(expr) : expr;
    return *this;
}

shared_ptr<ResultSet>
Query::execute() {
    return db_->query(*this);
}

} // namespace db
} // namespace brfc
