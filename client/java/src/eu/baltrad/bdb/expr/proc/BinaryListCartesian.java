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

import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ListExpression;

/**
 * Apply a binary operator when lhs and/or rhs are lists.
 * 
 * The result can be illustrated with operator +:
 *  (+ (1 2) 1) yields (2 3)
 *  (+ 1 (1 2)) yields (2 3)
 *  (+ (1 2) (3 4)) yields (4 5 5 6)
 *  (+ 1 2) yields 3
 */
public class BinaryListCartesian extends BinaryOperator {
  private final BinaryOperator op;

  public BinaryListCartesian(BinaryOperator op) {
    this.op = op;
  }

  @Override
  public Expression executeBinary(Expression lhs, Expression rhs) {
    if (lhs.isList() && !rhs.isList()) {
      return execute_lhsList(lhs, rhs);
    } else if (!lhs.isList() && rhs.isList()) {
      return execute_rhsList(lhs, rhs);
    } else if (lhs.isList() && rhs.isList()) {
      return execute_bothList(lhs, rhs);
    } else {
      return op.executeBinary(lhs, rhs);
    }
  }
  
  protected Expression execute_lhsList(Expression lhsList, Expression rhs) {
    ListExpression result = new ListExpression();
    for (Expression lhsExpr : lhsList) {
      result.add(op.executeBinary(lhsExpr, rhs));
    }
    return result;
  }

  protected Expression execute_rhsList(Expression lhs, Expression rhsList) {
    ListExpression result = new ListExpression();
    for (Expression rhsExpr : rhsList) {
      result.add(op.executeBinary(lhs, rhsExpr));
    }
    return result;
  }

  protected Expression execute_bothList(Expression lhsList, Expression rhsList) {
    ListExpression result = new ListExpression();
    for (Expression lhsExpr : lhsList) {
      for (Expression rhsExpr : rhsList) {
        result.add(op.executeBinary(lhsExpr, rhsExpr));
      }
    }
    return result;
  }
}
