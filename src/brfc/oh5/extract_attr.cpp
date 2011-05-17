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

#include <brfc/oh5/extract_attr.hpp>

#include <stdexcept>

#include <boost/algorithm/string/predicate.hpp>

#include <brfc/assert.hpp>

#include <brfc/expr/Expression.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

Expression
extract_attr::operator()(const Expression& args) const {
    BRFC_ASSERT(args.size() == 2);
    Expression::const_iterator it = args.begin();
    BRFC_ASSERT(it->is_string());
    const std::string& attrpath = it->string();
    ++it;
    BRFC_ASSERT(it->is_string());
    const std::string& attrtype = it->string();

    if (boost::starts_with(attrpath, "what/source:")) {
        return find_source(attrpath);
    } else {
        return find_all(attrpath, attrtype);
    }
}

Expression
extract_attr::find_source(const std::string& path) const {
    Expression result;
    const Source& s = file_->source();
    const std::string& key = path.substr(path.find_first_of(':') + 1);
    if (s.has(key)) {
        result.push_back(Expression(s.get(key)));
    }
    return result;
}

Expression
extract_attr::find_all(const std::string& path,
                       const std::string& type) const {
    Node::const_iterator it = file_->root().begin();
    Node::const_iterator end = file_->root().end();
    
    Expression result;
    for ( ; it != end; ++it) {
        const Attribute* a = it->child<Attribute>(path);
        if (a) {
            const Expression& e = value_expression(a->value(), type);
            if (e)
                result.push_back(e);
        }
    }

    return result;
}

Expression
extract_attr::value_expression(const Scalar& value,
                               const std::string& type) const {
    if (type == "string") {
        return Expression(value.string());
    } else if (type == "int64") {
        return Expression(value.int64_());
    } else if (type == "double") {
        return Expression(value.double_());
    } else if (type== "bool") {
        return Expression(value.to_bool());
    } else if (type == "date") {
        return Expression(value.to_date());
    } else if (type == "time") {
        return Expression(value.to_time());
    } else {
        throw std::logic_error("unknown attribute typename: " + type);
    }
}

} // namespace oh5
} // namespace brfc
