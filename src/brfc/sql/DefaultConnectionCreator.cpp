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

#include <brfc/sql/DefaultConnectionCreator.hpp>

#include <brfc/Url.hpp>

#include <brfc/sql/pg/Connection.hpp>

namespace brfc {
namespace sql {

DefaultConnectionCreator::DefaultConnectionCreator(const std::string& dsn)
        : dsn_(dsn) {
}

DefaultConnectionCreator::~DefaultConnectionCreator() {

}

Connection*
DefaultConnectionCreator::do_create() const {
    // rdb is our only implementation
    Url url(dsn_);
    if (url.scheme() == "postgresql")
        return new pg::Connection(url);
    else
        throw value_error("no mapping found for dsn scheme: " + url.scheme());

}

DefaultConnectionCreator*
DefaultConnectionCreator::do_clone() const {
    return new DefaultConnectionCreator(dsn_);
}

} // namespace sql
} // namespace brfc
