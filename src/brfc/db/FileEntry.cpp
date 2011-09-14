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

#include <brfc/db/FileEntry.hpp>

#include <brfc/DateTime.hpp>
#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

DateTime
FileEntry::stored_at() const {
    return do_stored_at();
}

Oh5Source
FileEntry::source() const {
    return do_source();
}

} // namespace brfc
