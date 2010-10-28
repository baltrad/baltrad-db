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

#include <brfc/db/rdb/RdbAttributeResult.hpp>

#include <brfc/Variant.hpp>

#include <brfc/sql/Result.hpp>

namespace brfc {
namespace db {
namespace rdb {

bool
RdbAttributeResult::do_next() {
    return result_->next();
}

bool
RdbAttributeResult::do_seek(int idx) {
    return result_->seek(idx);
}

int
RdbAttributeResult::do_size() const {
    return result_->size();
}

Variant
RdbAttributeResult::do_value_at(unsigned int pos) const {
    return result_->value_at(pos);
}

} // namespace rdb
} // namespace db
} // namespace brfc

