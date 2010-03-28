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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_ASSERT_H
#define BRFC_ASSERT_H

#include <stdexcept>
#include <string>

#include <boost/lexical_cast.hpp>

namespace brfc {

class assertion_error : public std::runtime_error {
  public:
    assertion_error(const std::string& msg)
            : std::runtime_error(msg) {
    }
};

inline void assert_fn(bool expr,
                      const std::string& expr_str,
                      const std::string& file,
                      unsigned int line) {
    if (!expr)
        throw assertion_error(file + ":" + 
                              boost::lexical_cast<std::string>(line) +
                              "   expr: " + expr_str);
}

}

#define BRFC_ASSERT(expr) assert_fn(expr, #expr, __FILE__, __LINE__);

#endif // BRFC_ASSERT_H

