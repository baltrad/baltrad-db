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

package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Node;
import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.oh5.Group;
import eu.baltrad.bdb.oh5.Dataset;

import org.apache.commons.lang3.StringUtils;

import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.node.ArrayNode;
import org.codehaus.jackson.node.BooleanNode;
import org.codehaus.jackson.node.DoubleNode;
import org.codehaus.jackson.node.JsonNodeFactory;
import org.codehaus.jackson.node.LongNode;
import org.codehaus.jackson.node.ObjectNode;
import org.codehaus.jackson.node.TextNode;

import java.io.InputStream;
import java.io.IOException;

public final class JsonUtil {
  private JsonNodeFactory nodeFactory;
  private ObjectMapper jsonMapper;

  public JsonUtil() {
    this.nodeFactory = JsonNodeFactory.instance;
    this.jsonMapper = new ObjectMapper();
  }

  public Metadata createOh5Metadata(JsonNode node) {
    Metadata metadata = new Metadata();
    for (JsonNode childNode : node) {
      addOh5Node(metadata, childNode);
    }
    return metadata;
  }

  public void addOh5Node(Metadata metadata, JsonNode jsonNode) {
    String path = jsonNode.get("path").getValueAsText();

    if ("/".equals(path)) {
      return;
    }

    String nodeName = StringUtils.substringAfterLast(path, "/");
    String parentPath = StringUtils.substringBeforeLast(path, "/");

    Node node = createOh5Node(nodeName, jsonNode);
    Node parent = metadata.getNode(parentPath);
    if (parent == null) {
      throw new RuntimeException(
        "could not find metadata node by path '" + path + "'"
      );
    }
    parent.addChild(node);
  }

  public Node createOh5Node(String name, JsonNode jsonNode) {
    String type = jsonNode.get("type").getValueAsText();
    if ("attribute".equals(type)) {
      return createOh5Attribute(name, jsonNode);
    } else if ("group".equals(type)) {
      return new Group(name);
    } else if ("dataset".equals(type)) {
      return new Dataset(name);
    } else {
      throw new RuntimeException("unhandled node type: " + type);
    }
  }

  public Node createOh5Attribute(String name, JsonNode jsonNode) {
    JsonNode valueNode = jsonNode.get("value");
    if (valueNode == null) {
      throw new RuntimeException("no value associated with attribute");
    } else if (valueNode.isFloatingPointNumber()) {
      return new Attribute(name, valueNode.getDoubleValue());
    } else if (valueNode.isIntegralNumber()) {
      return new Attribute(name, valueNode.getLongValue());
    } else if (valueNode.isTextual()) {
      return new Attribute(name, valueNode.getTextValue());
    } else {
      throw new RuntimeException(
        "attribute " + name + " unhandled JSON value node: "
        + valueNode.getClass().getName()
      );
    }
  }

  public JsonNode toJson(Expression expr) {
    switch (expr.getType()) {
      case LIST:
        ArrayNode listArray = nodeFactory.arrayNode();
        listArray.add(nodeFactory.textNode("list"));
        for (Expression childExpr : expr) {
          listArray.add(toJson(childExpr));
        }
        return listArray;
      case LONG:
        return nodeFactory.numberNode(expr.toLong());
      case DOUBLE:
        return nodeFactory.numberNode(expr.toDouble());
      case BOOLEAN:
        return nodeFactory.booleanNode(expr.toBoolean());
      case STRING:
        return nodeFactory.textNode(expr.toString());
      case SYMBOL:
        ArrayNode symbolArray = nodeFactory.arrayNode();
        symbolArray.add(nodeFactory.textNode("symbol"));
        symbolArray.add(nodeFactory.textNode(expr.toString()));
        return symbolArray;
      default:
        throw new RuntimeException(
          "uhandled expression type: " + expr.getClass().getName()
        );
    }
  }

  public JsonNode toJson(FileQuery query) {
    ObjectNode result = nodeFactory.objectNode();
    if (query.getFilter() != null) {
      result.put("filter", toJson(query.getFilter()));
    }
    ArrayNode order = nodeFactory.arrayNode();
    for (Expression expr : query.getOrderClause()) {
      order.add(toJson(expr));
    }
    result.put("order", order);
    if (query.getLimit() != null) {
      result.put("limit", query.getLimit());
    }
    if (query.getSkip() != null) {
      result.put("skip", query.getSkip());
    }
    return result;
  }

  public JsonNode toJson(AttributeQuery query) {
    ObjectNode result = nodeFactory.objectNode();

    ObjectNode fetch = nodeFactory.objectNode();
    for (String key : query.getFetchKeys()) {
      fetch.put(key, toJson(query.getFetchExpression(key)));
    }
    result.put("fetch", fetch);

    if (query.getFilter() != null) {
      result.put("filter", toJson(query.getFilter()));
    }

    ArrayNode order = nodeFactory.arrayNode();
    for (Expression expr : query.getOrderClause()) {
      order.add(toJson(expr));
    }
    if (order.size() > 0)
      result.put("order", order);

    ArrayNode group = nodeFactory.arrayNode();
    for (Expression expr : query.getGroupClause()) {
      group.add(toJson(expr));
    }
    if (group.size() > 0)
      result.put("group", group);

    if (query.getLimit() != null) {
      result.put("limit", query.getLimit());
    }

    if (query.getSkip() != null) {
      result.put("skip", query.getSkip());
    }

    return result;
  }

  public String jsonToString(JsonNode node) {
    try {
      return jsonMapper.writeValueAsString(node);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  public JsonNode jsonFromStream(InputStream input) {
    try {
      return jsonMapper.readTree(input);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }
}
