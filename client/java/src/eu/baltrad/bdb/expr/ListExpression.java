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

import org.apache.commons.lang3.StringUtils;

import java.util.*;

public class ListExpression extends BaseExpression {
  private List<Expression> values;

  public ListExpression() {
    values = new ArrayList<Expression>();
  }

  public ListExpression(List<Expression> values) {
    this();
    for (Expression value: values) {
      add(value);
    }
  }

  public ListExpression(Expression... values) {
    this(Arrays.asList(values));
  }

  public ListExpression(ListExpression other) {
    this(other.values);
  }

  @Override
  public Type getType() {
    return Type.LIST;
  }

  @Override
  public Expression get(int index) {
    return values.get(index);
  }

  @Override
  public Expression popFirst() {
    return values.remove(0);
  }

  @Override
  public int size() {
    return values.size();
  }

  @Override
  public Iterator<Expression> iterator() {
    return values.iterator();
  }

  @Override
  public String toString() {
    return "(" + StringUtils.join(values, ' ') + ")";
  }

  @Override
  public Boolean toBoolean() {
    return !values.isEmpty();
  }
  
  /**
   * Add a value to this list.
   *
   * @throws IllegalArgumentException if value is null
   */
  public void add(Expression value) {
    if (value == null)
      throw new IllegalArgumentException(
        "ExpressionList can't contain null values"
      );
    this.values.add(value);
  }

  @Override
  public boolean equals(Object other) {
    if (this == other)
      return true;
    
    if (!(other instanceof ListExpression))
      return false;
    
    ListExpression that = (ListExpression)(other);
    return this.values.equals(that.values);
  }

  @Override
  public int hashCode() {
    return values.hashCode();
  }
}
