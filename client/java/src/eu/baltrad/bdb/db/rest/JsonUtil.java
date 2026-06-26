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
import eu.baltrad.bdb.oh5.Source;

import org.apache.commons.lang3.StringUtils;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.BooleanNode;
import com.fasterxml.jackson.databind.node.DoubleNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.LongNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;

import java.io.InputStream;
import java.io.IOException;
import java.util.Collection;
import java.util.Iterator;

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
    String path = jsonNode.get("path").asText();

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
    String type = jsonNode.get("type").asText();
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

  public Attribute createOh5Attribute(String name, JsonNode jsonNode) {
    JsonNode valueNode = jsonNode.get("value");
    if (valueNode == null) {
      throw new RuntimeException("no value associated with attribute");
    } else if (valueNode.isFloatingPointNumber()) {
      return new Attribute(name, valueNode.asDouble());
    } else if (valueNode.isIntegralNumber()) {
      return new Attribute(name, valueNode.asLong());
    } else if (valueNode.isTextual()) {
      return new Attribute(name, valueNode.asText());
    } else if (valueNode.isArray()) {
      // XXX: properly handle arrays!
      return new Attribute(name);
    } else if (valueNode.isNull()) {
      return new Attribute(name);
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
      result.set("filter", toJson(query.getFilter()));
    }
    ArrayNode order = nodeFactory.arrayNode();
    for (Expression expr : query.getOrderClause()) {
      order.add(toJson(expr));
    }
    result.set("order", order);
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
      fetch.set(key, toJson(query.getFetchExpression(key)));
    }
    result.set("fetch", fetch);

    if (query.getFilter() != null) {
      result.set("filter", toJson(query.getFilter()));
    }

    ArrayNode order = nodeFactory.arrayNode();
    for (Expression expr : query.getOrderClause()) {
      order.add(toJson(expr));
    }
    if (order.size() > 0)
      result.set("order", order);

    ArrayNode group = nodeFactory.arrayNode();
    for (Expression expr : query.getGroupClause()) {
      group.add(toJson(expr));
    }
    if (group.size() > 0)
      result.set("group", group);

    if (query.getLimit() != null) {
      result.put("limit", query.getLimit());
    }

    if (query.getSkip() != null) {
      result.put("skip", query.getSkip());
    }
    
    if (query.getDistinct() != null) {
      result.put("distinct", query.getDistinct());
    }

    return result;
  }

  public JsonNode toJson(Source source) {
    ObjectNode result = nodeFactory.objectNode();
    ObjectNode src = nodeFactory.objectNode();
    ObjectNode values = nodeFactory.objectNode();
    
    Collection<String> keys = source.getKeys();
    for (String key : keys) {
      values.put(key, source.get(key));
    }
    
    src.put("name", source.getName());
    src.set("values", values);
    
    result.set("source", src);
    
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
