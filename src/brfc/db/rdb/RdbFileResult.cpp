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

#include <memory>

#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbFileResult.hpp>

#include <brfc/sql/Result.hpp>

namespace brfc {
namespace db {
namespace rdb {

bool
RdbFileResult::do_next() {
    return result_->next();
}

bool
RdbFileResult::do_seek(int idx) {
    return result_->seek(idx);
}

int
RdbFileResult::do_size() const {
    return result_->size();
}

FileEntry*
RdbFileResult::do_entry() {
    long long id = result_->value_at(0).int64_();

    auto_ptr<RdbFileEntry> entry(new RdbFileEntry(rdb_));
    entry->id(id);
    return entry.release();
}

} // namespace rdb
} // namespace db
} // namespace brfc
