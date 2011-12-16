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
import eu.baltrad.bdb.expr.BooleanExpression;

/**
 * Apply a binary operator when lhs and/or rhs are lists.
 * 
 * The result can be illustrated with operator =:
 *  (= (1 2) 3) yields false
 *  (= (1 2) 2) yields true
 *  (= 3 (1 2)) yields false
 *  (= 2 (1 2)) yields true
 *  (= (1 2) (3 4) yields false
 *  (= (1 2) (2 3) yields true
 *  (= 1 2) yields false
 *  (= 1 1) yields true
 *
 *  (in 'bropo' (attr 'how/task'))
 */
public class BinaryListFilter extends BinaryOperator {
  private final BinaryOperator op;

  public BinaryListFilter(BinaryOperator op) {
    this.op = op;
  }

  @Override
  public Expression executeBinary(Expression lhs, Expression rhs) {
    boolean result = false;
    if (lhs.isList() && !rhs.isList()) {
      result = execute_lhsList(lhs, rhs);
    } else if (!lhs.isList() && rhs.isList()) {
      result = execute_rhsList(lhs, rhs);
    } else if (lhs.isList() && rhs.isList()) {
      result = execute_bothList(lhs, rhs);
    } else {
      result = op.executeBinary(lhs, rhs).toBoolean().booleanValue();
    }
    return new BooleanExpression(result);
  }
  
  protected boolean execute_lhsList(Expression lhsList, Expression rhs) {
    for (Expression lhsExpr : lhsList) {
      if (op.executeBinary(lhsExpr, rhs).toBoolean()) {
        return true;
      }
    }
    return false;
  }

  protected boolean execute_rhsList(Expression lhs, Expression rhsList) {
    for (Expression rhsExpr : rhsList) {
      if (op.executeBinary(lhs, rhsExpr).toBoolean()) {
        return true;
      }
    }
    return false;
  }

  protected boolean execute_bothList(Expression lhsList, Expression rhsList) {
    for (Expression lhsExpr : lhsList) {
      for (Expression rhsExpr : rhsList) {
        if (op.executeBinary(lhsExpr, rhsExpr).toBoolean()) {
          return true;
        }
      }
    }
    return false;
  }
}
