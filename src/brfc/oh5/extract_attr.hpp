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
#ifndef BRFC_OH5_EXTRACT_ATTR_HPP
#define BRFC_OH5_EXTRACT_ATTR_HPP

#include <string>

namespace brfc {

namespace expr {
    class Expression;
}

namespace oh5 {

class File;
class Scalar;

/**
 * @brief extract attribute values from file
 */
struct extract_attr {
    typedef expr::Expression result_type;

    extract_attr() : file_(0) { }
    
    /**
     * @param args `(attrpath, attrtype)`
     * @return (attrval1, attrval2, ..., attrvalN)
     */
    expr::Expression operator()(const expr::Expression& args) const;

    expr::Expression find_source(const std::string& path) const;

    expr::Expression find_all(const std::string& path,
                              const std::string& type) const;

    expr::Expression value_expression(const Scalar& value,
                                      const std::string& type) const;

    void file(const File* f) { file_ = f; }

    const File* file_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_EXTRACT_ATTR_HPP
