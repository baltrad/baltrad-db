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

#ifndef BRFC_EXPR_LABEL_HPP
#define BRFC_EXPR_LABEL_HPP

#include <brfc/expr/Expression.hpp>

#include <QtCore/QString>

namespace brfc {
namespace expr {

/**
 * @brief labeled expression
 *
 * table.column AS label
 *
 * @ingroup exposed_in_binds
 */
class Label : public Expression {
  public:
    static LabelPtr create(ExpressionPtr expression, const QString& name) {
        return LabelPtr(new Label(expression, name));
    }

    void name(const QString& name) {
        name_ = name;
    }

    const QString& name() const {
        return name_;
    }

    void expression(ExpressionPtr expression) {
        expression_ = expression;
    }

    /**
     * @brief get expression this label is for
     */
    ExpressionPtr expression() const {
        return expression_;
    }

  protected:
    explicit Label(ExpressionPtr expression, const QString& name)
            : expression_(expression)
            , name_(name) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    ExpressionPtr expression_;
    QString name_;
};

}
}

#endif // BRFC_EXPR_LABEL_HPP
