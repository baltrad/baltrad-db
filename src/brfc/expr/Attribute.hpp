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

#ifndef BRFC_EXPR_ATTRIBUTE_HPP
#define BRFC_EXPR_ATTRIBUTE_HPP

#include <string>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

/**
 * @brief placeholder for HDF5 attributes
 */
class Attribute : public Expression {
  public:
    enum Type {
        UNKNOWN,
        STRING,
        INT64,
        LONG = INT64,
        DOUBLE,
        BOOL,
        DATE,
        TIME,
        DATETIME,
        SEQUENCE = STRING
    };

    Attribute(const std::string& name, Type type)
            : name_(name)
            , type_(type) {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new Attribute(*this));
    }

    const std::string& name() const {
        return name_;
    }

    Type type() const {
        return type_;
    }

  protected:
    Attribute(const Attribute& other)
            : name_(other.name_)
            , type_(other.type_) {
    }

    bool do_equals(const Expression& other) const {
        const Attribute* optr = dynamic_cast<const Attribute*>(&other);
        if (optr and name_ == optr->name_ and type_ == optr->type_)
            return true;
        return false;
    }

  private:
    std::string name_;
    Type type_;
};

}
}

#endif // BRFC_EXPR_ATTRIBUTE_HPP
