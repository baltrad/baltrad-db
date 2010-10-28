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
        , filter_() {
}

FileQuery::FileQuery(const FileQuery& other)
        : db_(other.db_)
        , filter_(other.filter_) {

}

FileQuery::~FileQuery() {
    
}

FileQuery&
FileQuery::filter(expr::ExpressionPtr expr) {
    filter_ = filter_ ? filter_->and_(expr) : expr;
    return *this;
}

shared_ptr<FileResult>
FileQuery::execute() {
    return db_->execute(*this);
}

} // namespace db
} // namespace brfc
