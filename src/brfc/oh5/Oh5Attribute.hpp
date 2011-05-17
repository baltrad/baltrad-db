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

#ifndef BRFC_OH5_ATTRIBUTE_H
#define BRFC_OH5_ATTRIBUTE_H

#include <brfc/oh5/Oh5Node.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

namespace brfc {

class Oh5Group;

/**
 * @brief ODIM_H5 attribute element
 */
class Oh5Attribute : public Oh5Node {
  public:
    /**
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     */
    Oh5Attribute(const std::string& name, const Oh5Scalar& value);

    /**
     * @brief destructor
     */
    virtual ~Oh5Attribute();

    /**
     * @brief attribute value
     */
    const Oh5Scalar& value() const { return value_; }

    /**
     * @brief set attribute value
     */
    void value(const Oh5Scalar& value) { value_ = value; }

    /**
     * @brief full name of this attribute
     *
     * if the parent group is named "what", "where" or "how", return
     * name prefixed with parent group, otherwise return name
     */
    std::string full_name() const;
  
  protected:
    Oh5Attribute(const Oh5Attribute& other);
    
  private:
    /**
     * @return false
     *
     * Attribute is last in the hierarchy and doesn't accept any children 
     */
    virtual bool do_accepts_child(const Oh5Node& node) const {
        return false;
    }
    
    virtual Oh5Node* do_clone() const;

    Oh5Scalar value_;
};

} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_H
