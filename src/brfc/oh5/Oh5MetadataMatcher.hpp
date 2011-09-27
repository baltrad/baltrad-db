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
#ifndef BRFC_OH5_METADATA_MATCHER_HPP
#define BRFC_OH5_METADATA_MATCHER_HPP

#include <brfc/expr/Eval.hpp>
#include <brfc/oh5/extract_attr.hpp>

namespace brfc {

class Oh5Metadata;

/**
 * @brief match files against filters
 *
 * @ingroup exposed
 */
class Oh5MetadataMatcher {
  public:
    Oh5MetadataMatcher();
    
    /**
     * @brief test if @c metadata matches @c x
     * @return true if @c x evaluates to true on @c file
     */
    bool match(const Oh5Metadata& metadata, const Expression& x);
    
  private:
    extract_attr attr_;
    Eval eval_;
};

} // namespace brfc

#endif // BRFC_OH5_METADATA_MATCHER_HPP