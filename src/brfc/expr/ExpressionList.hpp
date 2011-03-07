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
#ifndef BRFC_EXPR_EXPRESSION_LIST_HPP
#define BRFC_EXPR_EXPRESSION_LIST_HPP

#include <vector>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

/**
 * @brief list of expressions
 */
class ExpressionList : public Expression {
  public:
    ExpressionList()
            : exprs_() {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new ExpressionList(*this));
    }

    void append(const Expression& expr);

    const Expression& at(size_t pos) const { return *exprs_.at(pos); }

    size_t size() const { return exprs_.size(); }

  protected:
    ExpressionList(const ExpressionList& other);

    bool do_equals(const Expression& other) const;

  private:
    std::vector<ExpressionPtr> exprs_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_EXPRESSION_LIST_HPP
