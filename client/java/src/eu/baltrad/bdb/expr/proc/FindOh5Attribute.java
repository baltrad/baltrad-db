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
import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ListExpression;
import eu.baltrad.bdb.expr.LongExpression;
import eu.baltrad.bdb.expr.StringExpression;

import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Node;
import eu.baltrad.bdb.oh5.Source;

import org.apache.commons.lang3.StringUtils;

import java.util.*;

public final class FindOh5Attribute implements Procedure {
  private final Metadata metadata;

  protected enum AttributeType {
    STRING,
    LONG,
    DOUBLE,
    BOOLEAN
  }

  public FindOh5Attribute(Metadata metadata) {
    this.metadata = metadata;
  }

  @Override
  public Expression execute(Expression expr) {
    if (expr.size() != 2) {
      throw new EvaluationError
        ("invalid number of arguments (expected 2): " + expr.size()
      );
    }

    Expression attrPathExpr = expr.popFirst();
    if (!attrPathExpr.isString()) {
      throw new EvaluationError(
        "invalid type for attribute path (expected string): " +
        attrPathExpr.getClass().getName()
      );
    }
    
    Expression attrTypeExpr = expr.popFirst();
    if (!attrTypeExpr.isString()) {
      throw new EvaluationError(
        "invalid type for attribute type (expected string): " +
        attrPathExpr.getClass().getName()
      );
    }

    return findAll(attrPathExpr.toString(), attrTypeExpr.toString());
  }

  protected ListExpression findAll(String attrPath, String attrTypeString) {
    AttributeType attrType = AttributeType.valueOf(attrTypeString.toUpperCase());

    if (attrPath.startsWith("what/source:")) {
      return findSource(attrPath);
    } else {
      return findPlain(attrPath, attrType);
    }
  }

  protected ListExpression findSource(String attrPath) {
    ListExpression result = new ListExpression();
    String sourceKey = attrPath.substring(attrPath.lastIndexOf(':') + 1);
    Source source = metadata.getSource();
    if (source.has(sourceKey)) {
      result.add(new StringExpression(source.get(sourceKey)));
    }
    return result;
  }
  
  protected ListExpression findPlain(String attrPath,
                                     AttributeType attrType) {
    ListExpression result = new ListExpression();
    List<String> splitAttrPath =
      Arrays.asList(StringUtils.split(attrPath, '/'));
    for (Node node : metadata) {
      if (node instanceof Attribute) {
        if (matchPath(splitAttrPath, node.getPath())) {
          Attribute attrNode = (Attribute)node;
          Expression valueExpr = getAttributeValueExpression(attrNode, attrType);
          result.add(valueExpr);
        }
      }
    }
    return result;
  }
  
  /**
   * Check if nodePath contains attrPath.
   */
  protected boolean matchPath(List<String> attrPath, String nodePathString) {
    List<String> nodePath =
      Arrays.asList(StringUtils.split(nodePathString, '/'));
    
    int fromIndex = nodePath.size() - attrPath.size();
    // nodePath doesn't contain as many elements as we are looking for
    if (fromIndex < 0) {
      return false;
    }

    // pick the last elements from nodePath and compare them against attrPath
    List<String> nodePathTail = nodePath.subList(fromIndex, nodePath.size());
    return nodePathTail.equals(attrPath);
  }

  protected Expression getAttributeValueExpression(Attribute attr, AttributeType type) {
    switch (type) {
      case LONG:
        return new LongExpression(attr.toLong());
      case DOUBLE:
        return new DoubleExpression(attr.toDouble());
      case BOOLEAN:
        return new BooleanExpression(attr.toBoolean());
      case STRING:
        return new StringExpression(attr.toString());
      default:
        throw new RuntimeException("unhandled attribute type:" + type);
    }
  }
}
