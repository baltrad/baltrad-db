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

#ifndef BRFC_SMART_PTR_HPP
#define BRFC_SMART_PTR_HPP

#include <memory>

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace brfc {

using std::auto_ptr;

using boost::scoped_ptr;
using boost::shared_ptr;
using boost::weak_ptr;

using boost::enable_shared_from_this;
using boost::make_shared;

using boost::const_pointer_cast;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;

/**
 * @brief null deleter for shared_ptr
 */
void
no_delete(const void* ptr);

}

#endif // BRFC_SMART_PTR_HPP
