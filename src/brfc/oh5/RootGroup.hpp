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

#ifndef BRFC_OH5_ROOT_GROUP_HPP
#define BRFC_OH5_ROOT_GROUP_HPP

#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief the root group
 *
 * can have no parent
 * can have Attribute, AttributeGroup or DataSet as child
 */
class RootGroup : public Group {
  public:
    /**
     * @brief constructor
     */
    explicit RootGroup(File* file=0);

    virtual ~RootGroup();

    using Group::file;

    /**
     * @brief associate with a file
     */
    void file(File* file) {
        file_ = file;
    }

  protected:
    /**
     * accepts Attribute, AttributeGroup and Dataset as children
     */
    virtual bool do_accepts_child(const Node& node) const;

    /**
     * @return pointer to File or null if not associated
     */
    virtual const File* do_file() const {
        return file_;
    }
  
  private:
    File* file_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ROOT_GROUP_HPP
