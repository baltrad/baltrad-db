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

#include <brfc/expr/Attribute.hpp>

#include <stdexcept>

#include <brfc/expr/listcons.hpp>

namespace brfc {
namespace expr {

namespace {

std::string to_string(Attribute::Type t) {
    switch (t) {
        case Attribute::STRING:
            return "string";
        case Attribute::INT64:
            return "int64";
        case Attribute::DOUBLE:
            return "double";
        case Attribute::BOOL:
            return "bool";
        case Attribute::DATE:
            return "date";
        case Attribute::TIME:
            return "time";
        case Attribute::DATETIME:
            return "datetime";
        default:
            throw std::runtime_error("invalid attribute type");
    }
}

}

sexp
Attribute::to_sexp() const {
    // (type (attr "name"))
    return listcons().symbol("attr")
                     .string(name_)
                     .string(to_string(type_))
                     .get();
}

}
}
