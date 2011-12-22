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

import eu.baltrad.bdb.expr.BooleanExpression;
import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.Expression;

public final class OperatorIn extends BinaryOperator {
  private final Comparison compareEqual;

  public OperatorIn() {
    compareEqual = new Comparison(Comparison.Operator.EQ);
  }

  @Override
  protected Expression executeBinary(Expression lhs, Expression rhs) {
    if (!rhs.isList()) {
      throw new EvaluationError(
        "invalid type for operator in rhs (expected list): " +
        rhs.getClass().getName()
      );
    }

    for (Expression expr : rhs) {
      if (compareEqual.executeBinary(lhs, expr).toBoolean()) {
        return new BooleanExpression(true);
      }
    }

    return new BooleanExpression(false);
  }

}
