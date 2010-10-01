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
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     */
    Attribute(auto_ptr<NodeImpl> impl, const Scalar& value);

    /**
     * @brief destructor
     */
    virtual ~Attribute();

    /**
     * @brief return nearest parent Group that is not AttributeGroup
     * @return Group or null if not found
     *
     * if the attribute is in an AttributeGroup, bypass it
     *
     * @{
     */
    const Group* parent_group() const;

    Group* parent_group();
    /// @}
    
    /**
     * @brief attribute value
     */
    const Scalar& value() const { return value_; }

    /**
     * @brief set attribute value
     */
    void value(const Scalar& value) { value_ = value; }

    /**
     * @brief full name of this attribute
     *
     * if the attribute is in an AttributeGroup, return the name with
     * group name prepended.
     */
    String full_name() const;
    
  protected:
    /**
     * @return false
     *
     * Attribute is last in the hierarchy and doesn't accept any children 
     */
    virtual bool do_accepts_child(const Node& node) const {
        return false;
    }

  private:
    Scalar value_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_H
