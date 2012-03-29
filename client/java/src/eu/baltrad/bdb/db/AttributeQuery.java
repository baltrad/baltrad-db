package eu.baltrad.bdb.db;

import eu.baltrad.bdb.expr.Expression;

import java.util.*;

public class AttributeQuery {
  private Map<String, Expression> fetchMap;
  private Expression filter;
  private List<Expression> order;
  private List<Expression> group;
  private Integer limit;
  private Integer skip;
  private Boolean distinct;
  
  public AttributeQuery() {
    fetchMap = new HashMap<String, Expression>();
    filter = null;
    group = new ArrayList<Expression>();
    order = new ArrayList<Expression>();
    limit = null;
    skip = null;
  }

  public void fetch(String key, Expression expr) {
    fetchMap.put(key, expr);
  }

  public Collection<String> getFetchKeys() {
    return fetchMap.keySet();
  }

  public Expression getFetchExpression(String key) {
    return fetchMap.get(key);
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

  public List<Expression> getOrderClause() {
    return order;
  }

  public void appendGroupClause(Expression expr) {
    group.add(expr);
  }

  public List<Expression> getGroupClause() {
    return group;
  }

  public void setDistinct(Boolean distinct) {
	this.distinct = distinct;
  }

  public Boolean getDistinct() {
	return distinct;
  }
}
