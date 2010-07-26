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

#ifndef BRFC_SQL_LABEL_HPP
#define BRFC_SQL_LABEL_HPP

#include <brfc/String.hpp>

#include <brfc/sql/Expression.hpp>

namespace brfc {
namespace sql {

/**
 * @brief labeled expression
 *
 * table.column AS label
 */
class Label : public Expression {
  public:
    static LabelPtr create(ExpressionPtr expression, const String& name) {
        return LabelPtr(new Label(expression, name));
    }

    void name(const String& name) {
        name_ = name;
    }

    virtual const String& name() const {
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
    explicit Label(ExpressionPtr expression, const String& name)
            : expression_(expression)
            , name_(name) {
    }

  private:
    ExpressionPtr expression_;
    String name_;
};

}
}

#endif // BRFC_SQL_LABEL_HPP