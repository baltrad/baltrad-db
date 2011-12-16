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

public abstract class UnaryOperator implements Procedure {
  public final Expression execute(Expression expr) {
    if (expr.size() != 1) {
      throw new EvaluationError(
        "invalid number of arguments (expected 1): " + expr.size()
      );
    }
    Expression lhs = expr.popFirst();
    return executeUnary(lhs);
  }

  protected abstract Expression executeUnary(Expression expr);
}
