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

#include <algorithm>

#ifndef BRFC_UTIL_ALGORITHM_HPP
#define BRFC_UTIL_ALGORITHM_HPP

/**
 * @return true if [first, last) contains value
 */
template<typename InputIterator, typename T>
inline
bool
contains(InputIterator first, InputIterator last, const T& value) {
    return std::find(first, last, value) != last;
}

#endif // BRFC_UTIL_ALGORITHM_HPP
