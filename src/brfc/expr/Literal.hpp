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

#ifndef BRFC_EXPR_LITERAL_HPP
#define BRFC_EXPR_LITERAL_HPP

#include <brfc/expr/Expression.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace expr {

/**
 * @brief a literal value
 */
class Literal : public Expression {
  public:
    explicit Literal(const Variant& value)
            : value_(value) {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new Literal(*this));
    }

    const Variant& value() const {
        return value_;
    }

  protected:
    Literal(const Literal& other)
            : value_(other.value_) {
    }

    virtual bool do_equals(const Expression& other) const {
        const Literal* optr = dynamic_cast<const Literal*>(&other);
        if (optr and value_ == optr->value_)
            return true;
        return false;
    }

  private:
    Variant value_;
};

}
}

#endif // BRFC_EXPR_LITERAL_HPP
