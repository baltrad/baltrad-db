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

#ifndef BRFC_DB_RDB_SAVE_FILE_HPP
#define BRFC_DB_RDB_SAVE_FILE_HPP

#include <brfc/visit.hpp>

#include <brfc/db/rdb/GroupCache.hpp>
#include <brfc/db/rdb/SaveAttribute.hpp>
#include <brfc/db/rdb/SaveGroup.hpp>

namespace brfc {

namespace oh5 {

class PhysicalFile;
class AttributeGroup;
class Group;
class Attribute;

}

namespace db {
namespace rdb {

class RdbFileEntry;

/**
 * @brief save oh5::PhysicalFile instances to database
 */
class SaveFile {
  public:
    /**
     * @brief types accepted for visit()
     */
    typedef mpl::vector<const oh5::AttributeGroup,
                        const oh5::Group,
                        const oh5::Attribute> accepted_types;
    
    /**
     * @brief constructor
     * @param rdb database to save to
     */
    SaveFile(RelationalDatabase* rdb);
    
    /**
     * @brief save a oh5::AttributeGroup instance to database
     * 
     * this method does nothing, the group is contained in the
     * full name of the attribute.
     */
    void operator()(const oh5::AttributeGroup& group) {
        // no-op
    }
    
    /**
     * @brief save a oh5::Group instance to database
     *
     * save a group other than a oh5::AttributeGroup using a SaveGroup
     * functor.
     */
    void operator()(const oh5::Group& group);    
    
    /**
     * @brief save a oh5::Attribute instance to database
     *
     * instance is saved using a SaveAttribute functor.
     */
    void operator()(const oh5::Attribute& attribute);
    
    /**
     * @brief save a oh5::PhysicalFile instance to database
     * @param file the file to be saved
     * @return database id of the saved file
     */
    shared_ptr<RdbFileEntry> operator()(const oh5::PhysicalFile& file);

  private:
    RelationalDatabase* rdb_;
    GroupCache group_cache_;
    SaveAttribute save_attribute_;
    SaveGroup save_group_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_SAVE_FILE_HPP
