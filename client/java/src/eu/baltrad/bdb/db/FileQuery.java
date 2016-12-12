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

package eu.baltrad.bdb.db;

import eu.baltrad.bdb.expr.Expression;

import java.util.*;

public class FileQuery {
  private Expression filter;
  private List<Expression> order;
  private Integer limit;
  private Integer skip;
  private List<Expression> distinctOn;

  public FileQuery() {
    filter = null;
    order = new ArrayList<Expression>();
    limit = null;
    skip = null;
    distinctOn = new ArrayList<Expression>();
  }

  public void setFilter(Expression filter) {
    this.filter = filter;
  }

  public Expression getFilter() {
    return filter;
  }

  public void setLimit(Integer limit) {
    this.limit = limit;
  }

  public Integer getLimit() {
    return limit;
  }

  public void setSkip(Integer skip) {
    this.skip = skip;
  }

  public Integer getSkip() {
    return skip;
  }

  public void appendOrderClause(Expression expr) {
    order.add(expr);
  }

  public Collection<Expression> getOrderClause() {
    return order;
  }
  
  public void appendDistinctOnAttribute(Expression attribute) {
    distinctOn.add(attribute);
  }

  public Collection<Expression> getDistinctOnAttribute() {
    return distinctOn;
  }
}
