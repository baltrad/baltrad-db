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
import eu.baltrad.bdb.expr.DoubleExpression;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.LongExpression;
import eu.baltrad.bdb.expr.StringExpression;

public final class Comparison extends BinaryOperator {
  public enum Operator {
    EQ,
    NE,
    GT,
    GE,
    LT,
    LE
  }

  private final Operator op;

  public Comparison(Operator op) {
    this.op = op;
  }

  @Override
  protected Expression executeBinary(Expression lhs, Expression rhs) {
    int comparisonResult = 0;
    if (lhs.isNumber() && rhs.isNumber()) {
      if (lhs.isLong() && rhs.isLong()) {
        comparisonResult = compare(lhs.toLong(), rhs.toLong());
      } else {
        comparisonResult = compare(lhs.toDouble(), rhs.toDouble());
      }
    } else if (lhs.isString() && rhs.isString()) {
      comparisonResult = compare(lhs.toString(), rhs.toString());
    } else if (lhs.isBoolean() && rhs.isBoolean()) {
      comparisonResult = compare(lhs.toBoolean(), rhs.toBoolean());
    } else {
      throw new EvaluationError(
        "can't apply operator " + op + " to " + lhs + " and " + rhs
      );
    }
    return new BooleanExpression(comparisonResultToBoolean(comparisonResult));
  }

  protected int compare(Double lhs, Double rhs) {
    return lhs.compareTo(rhs);
  }

  protected int compare(Long lhs, Long rhs) {
    return lhs.compareTo(rhs);
  }

  protected int compare(String lhs, String rhs) {
    return lhs.compareTo(rhs);
  }

  protected int compare(Boolean lhs, Boolean rhs) {
    return lhs.compareTo(rhs);
  }

/*
  protected int compare(List<Expression> lhs, List<Expression> rhs) {
    for (int i = 0; i < lhs.size(); i++) {
      Expression leftElement = lhs.get(i);
      Expression rightElement = lhs.get(i);
      Expression elementResult = executeBinary(leftElement, rightElement);
      if (result.toBoolean()) {
        
      }
    }
  }
*/

  protected boolean comparisonResultToBoolean(int comparisonResult) {
    switch (op) {
      case EQ:
        return comparisonResult == 0;
      case NE:
        return comparisonResult != 0;
      case GT:
        return comparisonResult > 0;
      case GE:
        return comparisonResult >= 0;
      case LT:
        return comparisonResult < 0;
      case LE:
        return comparisonResult <= 0;
      default:
        throw new RuntimeException("unhandled comparison operator: " + op.toString());
    }
  }
}
