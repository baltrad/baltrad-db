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

#ifndef BRFC_RDB_ATTRIBUTE_SPECS_HPP
#define BRFC_RDB_ATTRIBUTE_SPECS_HPP

#include <map>

#include <brfc/String.hpp>

namespace brfc {
namespace rdb {

struct AttributeSpec {
    AttributeSpec(const String& name_,
                  const String& type_,
                  bool ignore_in_hash_=false)
            : name(name_)
            , type(type_)
            , ignore_in_hash(ignore_in_hash_) {
    }

    String name; ///< name of the Attribute
    String type; ///< type name of the attribute type
    bool ignore_in_hash; ///< ignore this attribute when hashing metadata
};

/**
 * @brief Attribute specifications
 *
 * contains valid attributes and their types
 */
class AttributeSpecs {
  public:
    /**
     * @brief construct empty container
     */
    AttributeSpecs();
    
    /**
     * @brief copy constuctor
     */
    AttributeSpecs(const AttributeSpecs& other);
    
    /**
     * @brief copy assignment
     */
    AttributeSpecs& operator=(const AttributeSpecs& rhs);

    /**
     * @brief destructor
     */
    ~AttributeSpecs();

    /**
     * @brief add a specification
     *
     * @throw value_error 
     */
    void add(const AttributeSpec& spec);

    bool has(const String& name) const;

    /**
     * @throw lookup_error if not found
     */
    const AttributeSpec& get(const String& name) const;

    void clear() { specs_.clear(); }

  private:
    typedef std::map<String, AttributeSpec> AttributeSpecMap;

    AttributeSpecMap specs_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_ATTRIBUTE_SPECS_HPP
