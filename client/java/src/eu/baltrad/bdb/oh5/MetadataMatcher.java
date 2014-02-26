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

package eu.baltrad.bdb.oh5;

import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ListExpression;
import eu.baltrad.bdb.expr.proc.BinaryListCartesian;
import eu.baltrad.bdb.expr.proc.BinaryListFilter;
import eu.baltrad.bdb.expr.proc.BinaryListIn;
import eu.baltrad.bdb.expr.proc.Comparison;
import eu.baltrad.bdb.expr.proc.FindOh5Attribute;
import eu.baltrad.bdb.expr.proc.MakeSymbol;
import eu.baltrad.bdb.expr.proc.OperatorAdd;
import eu.baltrad.bdb.expr.proc.OperatorDiv;
import eu.baltrad.bdb.expr.proc.OperatorLogicalAnd;
import eu.baltrad.bdb.expr.proc.OperatorLogicalOr;
import eu.baltrad.bdb.expr.proc.OperatorLogicalNot;
import eu.baltrad.bdb.expr.proc.OperatorMul;
import eu.baltrad.bdb.expr.proc.OperatorSub;
import eu.baltrad.bdb.expr.proc.Procedure;

import java.util.Map;
import java.util.HashMap;

public class MetadataMatcher {
  private Map<String, Procedure> procedures;

  public MetadataMatcher() {
    procedures = new HashMap<String, Procedure>();
    initProcedures();
  }
  
  protected void initProcedures() {
    procedures.put("+", new BinaryListCartesian(new OperatorAdd()));
    procedures.put("-", new BinaryListCartesian(new OperatorSub()));
    procedures.put("/", new BinaryListCartesian(new OperatorMul()));
    procedures.put("*", new BinaryListCartesian(new OperatorDiv()));

    procedures.put("=", new BinaryListFilter(
      new Comparison(Comparison.Operator.EQ)
    ));
    procedures.put("!=", new BinaryListFilter(
      new Comparison(Comparison.Operator.NE)
    ));
    procedures.put("<", new BinaryListFilter(
      new Comparison(Comparison.Operator.LT)
    ));
    procedures.put("<=", new BinaryListFilter(
      new Comparison(Comparison.Operator.LE)
    ));
    procedures.put(">", new BinaryListFilter(
      new Comparison(Comparison.Operator.GT)
    ));
    procedures.put(">=", new BinaryListFilter(
      new Comparison(Comparison.Operator.GE)
    ));

    procedures.put("and", new OperatorLogicalAnd());
    procedures.put("or", new OperatorLogicalOr());
    procedures.put("not", new OperatorLogicalNot());

    procedures.put("in", new BinaryListIn());

    procedures.put("symbol", new MakeSymbol());
  }

  /**
   * Test if metadata matches expr.
   * @return true if expr evaluates to true on file
   */
  public synchronized boolean match(Metadata metadata, Expression expr) {
    procedures.put("attr", new FindOh5Attribute(metadata));
    try {
      Expression result = eval(expr);
      return result.toBoolean();
    } finally {
      procedures.remove("attr");
    }
  }

  public synchronized Expression evalList(Expression args) {
    ListExpression result = new ListExpression();
    for (Expression arg : args) {
      result.add(eval(arg));
    }
    return result;
  }

  public synchronized Expression evalProcedure(Expression expr) {
    String procString = expr.popFirst().toString();
    Procedure procedure = procedures.get(procString);
    if (procedure == null) {
      throw new EvaluationError("no procedure found: " + procString);
    }
    Expression args = evalList(expr);
    return procedure.execute(args);
  }

  public synchronized Expression eval(Expression expr) {
    if (expr.isList()) {
      if (expr.size() > 0 && expr.get(0).isSymbol()) {
        return evalProcedure(expr);
      } else {
        return evalList(expr);
      }
    } else {
      return expr;
    }
  }
}
