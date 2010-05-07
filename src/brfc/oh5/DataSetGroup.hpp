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

#ifndef BRFC_OH5_DATA_SET_GROUP_HPP
#define BRFC_OH5_DATA_SET_GROUP_HPP

#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief ODIM_H5 dataset group
 *
 * can have Data or AttributeGroup or Quality as a child
 */
class DataSetGroup : public Group {
  public:
    virtual ~DataSetGroup();

  protected:
    // allow make_shared access to constructor
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);
    

    /**
     * @brief constructor
     */
    explicit DataSetGroup(const String& name);
    
    /**
     * @return true if node is Data or AttributeGroup
     */
    virtual bool do_accepts_child(const Node& node) const;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_DATA_SET_GROUP_HPP
