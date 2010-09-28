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

#ifndef BRFC_RDB_GROUP_ID_CACHE_HPP
#define BRFC_RDB_GROUP_ID_CACHE_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/rdb/Cache.hpp>
#include <brfc/oh5/Group.hpp>

namespace brfc {

namespace rdb {

class RelationalDatabase;

/**
 * @brief cache for stored oh5::Group instances
 */
class GroupCache : public Cache<long long, const oh5::Group*> {
  public:
    /**
     * @brief constructor
     *
     * @param rdb database this cache operates on
     */
    GroupCache(RelationalDatabase* rdb);

  protected:
    /**
     * @brief query for existing id from database
     */
    virtual OptionalKey do_lookup_key(const oh5::Group* group);

  private:
    RelationalDatabase* rdb_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_GROUP_ID_CACHE_HPP
