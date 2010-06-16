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

#ifndef BRFC_RDB_ATTRIBUTE_MAPPER_HPP
#define BRFC_RDB_ATTRIBUTE_MAPPER_HPP

#include <map>
#include <vector>

#include <brfc/String.hpp>

namespace brfc {
namespace rdb {

/**
 * @brief Attribute mapping to database table/column
 */
struct Mapping {
    Mapping(int id_,
            const String& attribute_,
            const String& type_,
            const String& table_,
            const String& column_,
            bool ignore_in_hash_=false)
            : id(id_)
            , attribute(attribute_)
            , type(type_)
            , table(table_)
            , column(column_)
            , ignore_in_hash(ignore_in_hash_) {
    }
    
    int id; ///< unique id
    String attribute; ///< Attribute name
    String type; ///< type name of the attribute
    String table; ///< table where the value is stored
    String column; ///< column in the table where value is stored
    bool ignore_in_hash; ///< ignore this attribute when hashing metadata
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
     * @brief is Attribute specialized
     * @param attribute Attribute name
     * @return true if attribute is found and is specialized
     * @throw lookup_error if not found
     *
     * attribute is specialized if it is stored outside the default table
     */
    bool is_specialized(const String& attribute) const;
    
    /**
     * @brief get specializations on table
     * @param table table name
     * @return MappingVector for Attributes that have their values stored
               in the table
     */
    MappingVector
    specializations_on(const String& table) const;

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

} // namespace rdb
} // namespace brfc

#endif //BRFC_RDB_ATTRIBUTE_MAPPER_HPP
