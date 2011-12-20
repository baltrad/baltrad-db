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

package eu.baltrad.bdb.expr;

import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.DateTime;
import eu.baltrad.bdb.util.Time;

import eu.baltrad.bdb.oh5.AttributeDefinitions;

import java.util.List;

public final class ExpressionFactory {
  private final ListCons listCons;
  private AttributeDefinitions attributeDefinitions;

  public ExpressionFactory() {
    this(AttributeDefinitions.getDefaultInstance());
  }

  public ExpressionFactory(AttributeDefinitions attributeDefinitions) {
    listCons = new ListCons();
    this.attributeDefinitions = attributeDefinitions;
  }

  public Expression binaryOperator(String op,
                                   Expression lhs,
                                   Expression rhs) {
    return listCons.symbol(op).append(lhs).append(rhs).create();
  }

  public Expression add(Expression lhs, Expression rhs) {
    return binaryOperator("+", lhs, rhs);
  }
  
  public Expression sub(Expression lhs, Expression rhs) {
    return binaryOperator("-", lhs, rhs);
  }

  public Expression mul(Expression lhs, Expression rhs) {
    return binaryOperator("*", lhs, rhs);
  }

  public Expression div(Expression lhs, Expression rhs) {
    return binaryOperator("/", lhs, rhs);
  }

  public Expression eq(Expression lhs, Expression rhs) {
    return binaryOperator("=", lhs, rhs);
  }

  public Expression ne(Expression lhs, Expression rhs) {
    return binaryOperator("!=", lhs, rhs);
  }

  public Expression lt(Expression lhs, Expression rhs) {
    return binaryOperator("<", lhs, rhs);
  }

  public Expression le(Expression lhs, Expression rhs) {
    return binaryOperator("<=", lhs, rhs);
  }

  public Expression gt(Expression lhs, Expression rhs) {
    return binaryOperator(">", lhs, rhs);
  }

  public Expression ge(Expression lhs, Expression rhs) {
    return binaryOperator(">=", lhs, rhs);
  }

  public Expression and(Expression lhs, Expression rhs) {
    return binaryOperator("and", lhs, rhs);
  }
  
  /**
   * join all expressions with AND.
   * 
   * @throws IllegalArgumentException when called with empty list
   */
  public Expression and(List<Expression> expressions) {
    if (expressions.isEmpty())
      throw new IllegalArgumentException("empty expression collection");
  
    Expression result = expressions.remove(0);
    for (Expression expr : expressions) {
      result = and(result, expr);
    }

    return result;
  }

  public Expression or(Expression lhs, Expression rhs) {
    return binaryOperator("or", lhs, rhs);
  }
  
  /**
   * join all expressions with OR.
   *
   * @throws IllegalArgumentException when called with empty list
   */
  public Expression or(List<Expression> expressions) {
    if (expressions.isEmpty())
      throw new IllegalArgumentException("empty expression collection");

    Expression result = expressions.remove(0);
    for (Expression expr : expressions) {
      result = or(result, expr);
    }

    return result;
  }

  public Expression not(Expression expr) {
    return listCons.symbol("not").append(expr).create();
  }

  public Expression like(Expression lhs, Expression rhs) {
    return listCons.symbol("like").append(lhs).append(rhs).create();
  }

  public Expression in(Expression lhs, Expression rhs) {
    return listCons.symbol("in").append(lhs).append(rhs).create();
  }

  public Expression list(Expression... exprs) {
    return listCons.extend(exprs).create();
  }

  public Expression list(List<Expression> exprs) {
    return listCons.extend(exprs).create();
  }

  public Expression attribute(String name, String type) {
    return listCons.symbol("attr").string(name).string(type).create();
  }
  
  /**
   * Create an attribute expression, looking up the type
   *
   * @throws IllegalArgumentException when no default type is defined for
   *           the attribute.
   */
  public Expression attribute(String name) {
    return attribute(name, findAttributeTypeFor(name));
  }
  
  protected String findAttributeTypeFor(String name) {
    return attributeDefinitions.getAttributeType(name);
  }

  public Expression asc(Expression expr) {
    return listCons.symbol("asc").append(expr).create();
  }

  public Expression desc(Expression expr) {
    return listCons.symbol("desc").append(expr).create();
  }

  public Expression literal(long value) {
    return new LongExpression(value);
  }

  public Expression literal(String value) {
    return new StringExpression(value);
  }

  public Expression literal(boolean value) {
    return new BooleanExpression(value);
  }

  public Expression literal(double value) {
    return new DoubleExpression(value);
  }

  public Expression literal(Date value) {
    return listCons.symbol("date")
                   .literal(value.year())
                   .literal(value.month())
                   .literal(value.day())
                   .create();
  }

  public Expression literal(Time value) {
    return listCons.symbol("time")
                   .literal(value.hour())
                   .literal(value.minute())
                   .literal(value.second())
                   .create();
  }

  public Expression literal(DateTime value) {
    Date date = value.getDate();
    Time time = value.getTime();
    return listCons.symbol("datetime")
                   .literal(date.year())
                   .literal(date.month())
                   .literal(date.day())
                   .literal(time.hour())
                   .literal(time.minute())
                   .literal(time.second())
                   .create();
  }

  public Expression symbol(String value) {
    return new SymbolExpression(value);
  }

  public Expression count(Expression expr) {
    return listCons.symbol("count").append(expr).create();
  }

  public Expression sum(Expression expr) {
    return listCons.symbol("sum").append(expr).create();
  }

  public Expression min(Expression expr) {
    return listCons.symbol("min").append(expr).create();
  }

  public Expression max(Expression expr) {
    return listCons.symbol("max").append(expr).create();
  }

  public Expression combinedDateTime(String dateAttr, String timeAttr) {
    return add(attribute(dateAttr, "date"), attribute(timeAttr, "time"));
  }
}
