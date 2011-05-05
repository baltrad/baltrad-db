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

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Select.hpp>

namespace {

template<typename T>
bool
contains(const T& container, const typename T::value_type& value) {
    return std::find(container.begin(), container.end(), value) != container.end();
}

} // namespace anonymous

namespace brfc {
namespace sql {

class sql_Select_test : public ::testing::Test {
  public:
    sql_Select_test() { }
    
};

} // namespace sql
} // namespace brfc
