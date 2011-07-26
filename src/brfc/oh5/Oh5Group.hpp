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

#ifndef BRFC_OH5_GROUP_HPP
#define BRFC_OH5_GROUP_HPP

#include <brfc/oh5/Oh5Node.hpp>

namespace brfc {

class Oh5Attribute;

/**
 * @brief ODIM_H5 Group element
 *
 * @ingroup exposed
 */
class Oh5Group : public Oh5Node {
  public:
    /**
     * @brief constructor
     */
    explicit Oh5Group(const std::string& name);

    /**
     * @brief destructor
     */
    virtual ~Oh5Group();
    
    /**
     * @brief access a child attribute "in effect"
     * @param name name of the attribute, can contain a group (e.g what/date)
     * @return pointer to Attribute or null if not found
     *
     * This method seeks for an attribute "in effect" at this hierarchy
     * level. The search is performed among immediate children (if prefixed
     * with a group name, also in respective group). If no match is found at
     * current level, the search is continued up the hierarchy until a match
     * is found or root node is reached.
     *
     * So given a hierarchy:
     *  - /
     *    - what
     *      - attr1
     *      - attr2
     *    - dataset1
     *      - attr1
     *      - data1
     *        - what
     *          - attr1
     *          - attr2
     *      - data2
     * 
     * following results would be obtained:
     * 
     * - search for attr1 @ data2 would return /dataset1/attr1
     * - search for attr2 @ data2 would return null
     * - search for what/attr1 @ data2 would return /what/attr1
     * - search for what/attr1 @ data1 would return /dataset1/data1/what/attr1
     */
    Oh5Attribute* effective_attribute(const std::string& name);
    
    /**
     * @copydoc effective_attribute()
     */
    const Oh5Attribute* effective_attribute(const std::string& name) const;
    
    /**
     * @brief access child group by path, trying to create missing groups
     * @throw std::invalid_argument if an invalid name is encountered in @c path
     * @throw std::invalid_argument if the path is absolute and this not is not root
     * @return reference to the last created group
     *
     * @note this is not atomic. If an error occurs when creating missing
     *       groups after the first, the groups that have already been
     *       created, stay.
     */
    Oh5Group& get_or_create_group(const std::string& path);
    
  protected:
    Oh5Group(const Oh5Group& other);
  
  private:
    virtual bool do_accepts_child(const Oh5Node& node) const;

    virtual Oh5Node* do_clone() const;
};

} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
