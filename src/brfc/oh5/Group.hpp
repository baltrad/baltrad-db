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
 * @brief ABC for ODIM_H5 elements that can have children
 */
class Group : public Node {
  public:
    /**
     * @brief create a Group subclass instance from name
     * @return pointer to instance or null pointer if name is not a group
     *
     * ODIM_H5 node names map as (where N is an integer):
     *  - datasetN -> @ref DataSetGroup
     *  - dataN -> @ref DataGroup
     *  - qualityN -> @ref QualityGroup
     *  - (what|where|how) -> @ref AttributeGroup
     */
    static shared_ptr<Group> create_by_name(const String& name);

    /**
     * @brief destructor
     */
    virtual ~Group();

    shared_ptr<Group> shared_from_this() {
        return static_pointer_cast<Group>(Node::shared_from_this());
    }

    shared_ptr<const Group> shared_from_this() const {
        return static_pointer_cast<const Group>(Node::shared_from_this());
    }
    
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
    shared_ptr<Attribute> child_attribute(const String& name);

    shared_ptr<const Attribute> child_attribute(const String& name) const;
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
    shared_ptr<Attribute> attribute(const String& name);

    shared_ptr<const Attribute> attribute(const String& name) const;
    ///@}
    
    /**
     * @{
     * @brief access child group by name
     * @param name name of the group
     * @return pointer to Group or null if not found
     *
     * The search for the group is performed among immediate children.
     */
    shared_ptr<Group> child_group_by_name(const String& name);

    shared_ptr<const Group> child_group_by_name(const String& name) const;
    ///@}
    
    /**
     * @brief access child group by name, trying to create one if missing
     * @return pointer to group or null if group can not be created
     *
     * @sa create_by_name()
     */
    shared_ptr<Group>
    get_or_create_child_group_by_name(const String& name);
    
    /**
     * @brief access child group by path, trying to create missing groups
     * @throw value_error if an invalid name is encountered in @c path
     * @return pointer to the last group in path or null if the path could
     *         not be created
     *
     * if a group in path can not be created, none of the groups are
     * attached to this group.
     * 
     * @sa get_or_create_child_group_by_name()
     */
    shared_ptr<Group>
    get_or_create_child_group_by_path(const StringList& path);
    
  protected:
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     */
    Group(const String& name)
        : Node(name) {
    }

    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual bool do_accepts_parent(const Node& node) const;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
