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

#ifndef BRFC_SQL_ELEMENT_HPP
#define BRFC_SQL_ELEMENT_HPP

#include <brfc/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace brfc {
namespace sql {

/**
 * @brief ABC for elements in sql statements
 */
class Element : public boost::noncopyable
              , public enable_shared_from_this<Element> {
  public:
    virtual ~Element() {}
    
  protected:
    Element()
            : boost::noncopyable()
            , enable_shared_from_this<Element>() {
    }
};


} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_ELEMENT_HPP
