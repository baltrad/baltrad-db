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

#ifndef BRFC_EXPR_JOIN_HPP
#define BRFC_EXPR_JOIN_HPP

#include <brfc/expr/fwd.hpp>
#include <brfc/expr/Selectable.hpp>

namespace brfc {
namespace expr {

class Join : public Selectable {
  public:
    enum Type {
        NONE = 0,
        INNER = 1,
        LEFT = 2
    };

    static JoinPtr create(SelectablePtr from,
                          SelectablePtr to,
                          ExpressionPtr condition,
                          Type type=INNER) {
        return JoinPtr(new Join(from, to, condition, type));
    }

    virtual QString name() const { return ""; }

    SelectablePtr from() const {
        return from_;
    }

    void from(SelectablePtr from) {
        from_ = from;
    }

    SelectablePtr to() const {
        return to_;
    }

    void to(SelectablePtr to) {
        to_ = to;
    }

    ExpressionPtr condition() const {
        return condition_;
    }

    void condition(ExpressionPtr condition) {
        condition_ = condition;
    }
    
    /**
     * @brief test if join contains a selectable element
     *
     * if this join is part of a chain, the check is made in the entire
     * chain. Selectable::name is used to determine equality.
     */
    bool contains(SelectablePtr element) const;

    Type type() const { return type_; }

  protected:
    Join(SelectablePtr from, SelectablePtr to, ExpressionPtr condition, Type type);

  private:
    SelectablePtr from_;
    SelectablePtr to_;
    ExpressionPtr condition_;
    Type type_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_JOIN_HPP
