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

#ifndef BRFC_OH5_GROUP_HPP
#define BRFC_OH5_GROUP_HPP

#include <brfc/oh5/Node.hpp>

namespace brfc {

class StringList;

namespace oh5 {

class Attribute;

/**
 * @brief ODIM_H5 Group element
 */
class Group : public Node {
  public:
    /**
     * @brief constructor
     */
    Group(Node* parent, const String& name)
        : Node(parent, name) {
    }

    /**
     * @brief create a Group subclass instance from name
     * @return pointer to instance or null pointer if name is not a group
     *
     * ODIM_H5 node names map by name (where N is an integer):
     *  - datasetN, dataN, qualityN -> @ref Group
     *  - (what|where|how) -> @ref AttributeGroup
     *  - everything else -> null pointer
     */
    static auto_ptr<Group> create_by_name(const String& name);

    /**
     * @brief destructor
     */
    virtual ~Group();

    /**
     * @{
     * @brief access a child attribute
     * @param name name of the attribute, can contain a group (e.g what/date)
     * @return pointer to Attribute or null if not found
     * 
     * The search for the attribute is performed among immediate children.
     * If the name is prefixed with a group, the search is performed in
     * the respective AttributeGroup.
     */
    Attribute* child_attribute(const String& name);

    const Attribute* child_attribute(const String& name) const;
    ///@}
    
    /**
     * @{
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
     * - and so on ...
     * 
     */
    Attribute* attribute(const String& name);

    const Attribute* attribute(const String& name) const;
    ///@}
    
    /**
     * @{
     * @brief access child group by name
     * @param name name of the group
     * @return pointer to Group or null if not found
     *
     * The search for the group is performed among immediate children.
     */
    Group* child_group_by_name(const String& name);

    const Group* child_group_by_name(const String& name) const;
    ///@}
    
    /**
     * @brief access child group by name, trying to create one if missing
     * @return reference to created group
     */
    Group&
    get_or_create_child_group_by_name(const String& name);
    
    /**
     * @brief access child group by path, trying to create missing groups
     * @throw value_error if an invalid name is encountered in @c path
     * @return reference to the last created group
     *
     * if a group in path can not be created, none of the groups are
     * attached to this group.
     * 
     * @sa get_or_create_child_group_by_name()
     */
    Group&
    get_or_create_child_group_by_path(const StringList& path);
    
  protected:
    virtual bool do_accepts_child(const Node& node) const;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
