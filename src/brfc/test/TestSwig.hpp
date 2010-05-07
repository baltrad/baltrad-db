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

#ifndef BRFC_TEST_SWIG_HPP
#define BRFC_TEST_SWIG_HPP

#include <string>

#include <brfc/String.hpp>

namespace brfc {
namespace test {

class TestSwig {
  public:
    std::string
    qstring_to_string(String str);

    std::string
    qstringconstref_to_string(const String& str);

    String
    string_to_qstring(const std::string& str);

    const String&
    string_to_qstringconstref(const std::string& str);

  private:
    String for_constref_;
};

} // namespace test
} // namespace brfc

#endif // BRFC_TEST_SWIG_HPP
