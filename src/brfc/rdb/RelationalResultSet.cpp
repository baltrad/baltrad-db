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

#include <brfc/rdb/RelationalResultSet.hpp>

#include <brfc/Variant.hpp>

#include <brfc/rdb/Result.hpp>

namespace brfc {
namespace rdb {

bool
RelationalResultSet::do_next() {
    return result_->next();
}

bool
RelationalResultSet::do_seek(int idx) {
    return result_->seek(idx);
}

int
RelationalResultSet::do_size() {
    return result_->size();
}

Variant
RelationalResultSet::do_value_at(unsigned int pos) const {
    return result_->value_at(pos);
}

} // namespace rdb
} // namespace brfc

