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
    static LiteralPtr create(const Variant& value) {
        return LiteralPtr(new Literal(value));
    }

/*
  XXX: this seems problematic, we need to be able to convert
       any brfc::Variant to string for this to work, but this
       is not implemented.

       This method seems not to be used anywhere, so, remove?.

    String string() const {
        return value_.string();
    }
*/
    const Variant& value() const {
        return value_;
    }

  protected:
    explicit Literal(const Variant& value)
            : value_(value) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    Variant value_;
};

}
}

#endif // BRFC_EXPR_LITERAL_HPP
