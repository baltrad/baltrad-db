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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_OH5_ATTRIBUTE_H
#define BRFC_OH5_ATTRIBUTE_H

#include <brfc/Variant.hpp>
#include <brfc/oh5/Node.hpp>

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
    const Variant& value() const { return value_; }

    /**
     * @brief set attribute value
     */
    void value(const Variant& value);

    /**
     * @brief is the attribute valid
     *
     * attribute is considered valid if it has a value (Variant is not null)
     */
    bool is_valid() const;

    /**
     * @brief full name of this attribute
     *
     * if the attribute is in an AttributeGroup, return the name with
     * group name prepended.
     */
    QString full_name() const;
    
    /**
     * @brief string representation of this attribute
     *
     * /path/to/attribute=value
     */
    QString to_string() const;

  protected:
    template<class T, class A1>
    friend
    shared_ptr<T> boost::make_shared(const A1&);

    template<class T, class A1, class A2> 
    friend 
    shared_ptr<T> boost::make_shared(const A1&, const A2&);

    /**
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     */
    explicit Attribute(const QString& name,
                       const Variant& value=Variant());

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
    Variant value_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_H
