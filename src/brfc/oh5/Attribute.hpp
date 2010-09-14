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

#ifndef BRFC_OH5_ATTRIBUTE_H
#define BRFC_OH5_ATTRIBUTE_H

#include <brfc/oh5/Node.hpp>
#include <brfc/oh5/Scalar.hpp>

namespace brfc {

namespace oh5 {

class Group;

/**
 * @brief ODIM_H5 attribute element
 */
class Attribute : public Node {
  public:
    /**
     * @brief destructor
     */
    virtual ~Attribute();

    shared_ptr<Attribute> shared_from_this() {
        return static_pointer_cast<Attribute>(Node::shared_from_this());
    }

    shared_ptr<const Attribute> shared_from_this() const {
        return static_pointer_cast<const Attribute>(Node::shared_from_this());
    }
    
    /**
     * @brief return nearest parent Group that is not AttributeGroup
     * @return Group or null if not found
     *
     * if the attribute is in an AttributeGroup, bypass it
     *
     * @{
     */
    shared_ptr<const Group> parent_group() const;

    shared_ptr<Group> parent_group();
    /// @}
    
    /**
     * @brief attribute value
     */
    const Scalar& value() const { return value_; }

    /**
     * @brief set attribute value
     */
    void value(const Scalar& value);

    /**
     * @brief full name of this attribute
     *
     * if the attribute is in an AttributeGroup, return the name with
     * group name prepended.
     */
    String full_name() const;
    
  protected:
    template<class T, class A1, class A2> 
    friend 
    shared_ptr<T> boost::make_shared(const A1&, const A2&);

    /**
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     */
    explicit Attribute(const String& name,
                       const Scalar& value);

    /**
     * @return false
     *
     * Attribute is last in the hierarchy and doesn't accept any children 
     */
    virtual bool do_accepts_child(const Node& node) const {
        return false;
    }

    /**
     * @return true
     *
     * relies on the parent node to deny it
     */
    virtual bool do_accepts_parent(const Node& node) const {
        return true;
    }

  private:
    Scalar value_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_H
