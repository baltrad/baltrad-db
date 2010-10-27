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

#ifndef BRFC_DB_RDB_ATTRIBUTE_MAPPER_HPP
#define BRFC_DB_RDB_ATTRIBUTE_MAPPER_HPP

#include <map>
#include <vector>

#include <brfc/String.hpp>
#include <brfc/sql/fwd.hpp>

namespace brfc {

class StringList;

namespace db {
namespace rdb {

/**
 * @brief Attribute mapping to database table/column
 */
struct Mapping {
    Mapping(const String& attribute_, sql::ColumnPtr column_)
            : attribute(attribute_)
            , column(column_) {
    }
    
    String attribute; ///< Attribute name
    sql::ColumnPtr column; ///< column in the table where value is stored
};

/**
 * @brief vector of Mappings
 */
typedef std::vector<Mapping> MappingVector;

/**
 * @brief map attributes to relational database (table, column)
 */
class AttributeMapper {
  public:
    /**
     * @brief default constructor
     */
    AttributeMapper();

    /**
     * @brief copy constructor
     */
    AttributeMapper(const AttributeMapper& other);

    /**
     * @brief copy assignment
     */
    AttributeMapper& operator=(const AttributeMapper& rhs);

    /**
     * @brief destructor
     */
    ~AttributeMapper();
    
    /**
     * @brief add a mapping
     *
     * @param mapping column in the table where Attribute value is stored
     * @throw duplicate_entry if a mapping for attribute is already defined
     */
    void add(const Mapping& mapping);
    
    /**
     * @brief get specializations on table
     * @param table table name
     * @return MappingVector for Attributes that have their values stored
               in the table
     */
    MappingVector
    specializations_on(sql::TablePtr table) const;

    /**
     * @brief is mapper aware of Attribute
     * @param attribute Attribute name
     * @return true if found
     */
    bool has(const String& attribute) const; 

    /**
     * @brief get Mapping
     * @param attribute Attribute name
     * @return Mapping for the Attribute
     * @throw lookup_error if not found
     */
    const Mapping& mapping(const String& attribute) const;

    void clear() { mappings_.clear(); }

  private:
    typedef std::map<String, Mapping> MappingMap;

    MappingMap mappings_;
};

} // namespace db
} // namespace rdb
} // namespace brfc

#endif //BRFC_DB_RDB_ATTRIBUTE_MAPPER_HPP
