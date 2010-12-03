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

#ifndef BRFC_EXPR_ATTRIBUTE_PROTOTYPES_HPP
#define BRFC_EXPR_ATTRIBUTE_PROTOTYPES_HPP

#include <map>
#include <string>

#include <brfc/expr/Attribute.hpp>

namespace brfc {
namespace expr {

class AttributePrototypes {
  public:
    AttributePrototypes()
            : prototypes_() {
    }
    
    /**
     * @brief default attribute prototypes
     *
     * Attributes defined in ODIM_H5 + "virtual" attributes
     */
    static AttributePrototypes default_odim_h5();

    bool has(const std::string& name) const;

    void add(const Attribute& prototype);

    const Attribute& get(const std::string& name) const;

    void clear();

  private:
    typedef std::map<std::string, AttributePtr> PrototypeMap;
    PrototypeMap prototypes_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_ATTRIBUTE_PROTOTYPES_HPP
