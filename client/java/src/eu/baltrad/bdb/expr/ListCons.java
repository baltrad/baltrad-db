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

import java.util.List;

public class ListCons {
  private ListExpression value;

  public ListCons() {
    value = new ListExpression();
  }

  public ListCons symbol(String value) {
    this.value.add(new SymbolExpression(value));
    return this;
  }

  public ListCons string(String value) {
    this.value.add(new StringExpression(value));
    return this;
  }

  public ListCons literal(long value) {
    this.value.add(new LongExpression(value));
    return this;
  }

  public ListCons append(Expression value) {
    this.value.add(value);
    return this;
  }

  public ListCons extend(Expression... values) {
    for (Expression value : values) {
      this.value.add(value);
    }
    return this;
  }

  public ListCons extend(List<Expression> values) {
    for (Expression value : values) {
      this.value.add(value);
    }
    return this;
  }

  public Expression create() {
    Expression result = new ListExpression(value);
    value = new ListExpression();
    return result;
  }

}
