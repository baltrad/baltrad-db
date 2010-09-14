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

#ifndef BRFC_RDB_SAVE_ATTRIBUTE_HPP
#define BRFC_RDB_SAVE_ATTRIBUTE_HPP

#include <brfc/sql/fwd.hpp>

namespace brfc {

namespace oh5 {

class Attribute;

}

namespace rdb {

class GroupCache;
class RelationalDatabase;

/**
 * @brief save oh5::Attribute instances to database
 */
class SaveAttribute {
  public:
    /**
     * @brief constrcutor
     * @param rdb database to save to
     * @param group_cache cache of group ids
     */
    SaveAttribute(RelationalDatabase* rdb, GroupCache* group_cache);
    
    /**
     * @brief save an oh5::Attribute instance to database
     *
     * Invalid attributes are inserted to dbd_invalid_attributes,
     * specialized attributes are ignored. All other attributes are saved
     * to the table they are mapped to.
     */
    void operator()(const oh5::Attribute& attr);

  private:
    sql::InsertPtr invalid_attribute_query(const oh5::Attribute& attr);
    sql::InsertPtr valid_attribute_query(const oh5::Attribute& attr);

    RelationalDatabase* rdb_;
    GroupCache* group_cache_;
};

sql::LiteralPtr
attr_sql_value(const oh5::Attribute& attr);

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SAVE_ATTRIBUTE_HPP
