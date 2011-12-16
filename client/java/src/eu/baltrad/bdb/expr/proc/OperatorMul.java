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

package eu.baltrad.bdb.expr.proc;

import eu.baltrad.bdb.expr.DoubleExpression;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.LongExpression;

public final class OperatorMul extends BinaryOperator {
  @Override
  protected Expression executeBinary(Expression lhs, Expression rhs) {
    Expression result = null;
    if (lhs.isNumber() && rhs.isNumber()) {
      if (lhs.isLong() && rhs.isLong()) {
        result = execute(lhs.toLong(), rhs.toLong());
      } else {
        result = execute(lhs.toDouble(), rhs.toDouble());
      }
    } else {
      throw new EvaluationError(
        "unsupported types for multiplication: " +
        lhs.getClass().getName() + ", " +
        rhs.getClass().getName()
      );
    }
    return result;
  }

  protected Expression execute(Double lhs, Double rhs) {
    return new DoubleExpression(lhs * rhs);
  }

  protected Expression execute(Long lhs, Long rhs) {
    return new LongExpression(lhs * rhs);
  }
}
