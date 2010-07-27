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
 * @brief a labeled SQL expression
 */
class Label : public Expression {
  public:
    /**
     * @brief construct as a shared_ptr
     * @sa Label()
     */
    static LabelPtr create(ExpressionPtr expression, const String& name) {
        return LabelPtr(new Label(expression, name));
    }
    
    /**
     * @brief the label
     */
    virtual const String& name() const {
        return name_;
    }

    /**
     * @brief labeled expression
     */
    ExpressionPtr expression() const {
        return expression_;
    }

  protected:
    /**
     * @brief constructor
     * @param expression the expression to label
     * @param name label name
     */
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
