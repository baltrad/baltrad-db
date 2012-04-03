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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.node.ArrayNode;
import org.codehaus.jackson.node.JsonNodeFactory;
import org.codehaus.jackson.node.ObjectNode;
import org.junit.Before;
import org.junit.Test;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;
import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;

public class JsonUtilTest {
  JsonNodeFactory nodeFactory;
  ExpressionFactory exprFactory;
  JsonUtil classUnderTest;

  @Before
  public void setUp() {
    nodeFactory = JsonNodeFactory.instance;
    exprFactory = new ExpressionFactory();
    classUnderTest = new JsonUtil();
  }

  @Test
  public void createOh5Metadata() {
    ArrayNode jsonMeta = nodeFactory.arrayNode();
    ObjectNode jsonObj = nodeFactory.objectNode();
    jsonObj.put("path", "/what");
    jsonObj.put("type", "group");
    jsonMeta.add(jsonObj);
    jsonObj = nodeFactory.objectNode();
    jsonObj.put("path", "/what/object");
    jsonObj.put("type", "attribute");
    jsonObj.put("value", "PVOL");
    jsonMeta.add(jsonObj);

    Metadata meta = classUnderTest.createOh5Metadata(jsonMeta);
    Attribute attr = meta.getAttribute("/what/object");
    assertNotNull(attr);
    assertEquals("PVOL", attr.toString());
  }

  @Test
  public void createOh5Attribute_arrayValue() {
    ObjectNode jsonObj = nodeFactory.objectNode();
    ArrayNode value = nodeFactory.arrayNode();
    jsonObj.put("value", value);

    Attribute result = classUnderTest.createOh5Attribute("attrname", jsonObj);
    assertTrue(result.isNull());
  }

  @Test
  public void createOh5Attribute_nullValue() {
    ObjectNode jsonObj = nodeFactory.objectNode();
    jsonObj.put("value", nodeFactory.nullNode());

    Attribute result = classUnderTest.createOh5Attribute("attrname", jsonObj);
    assertTrue(result.isNull());
  }

  @Test
  public void toJson_Expression() {
    Expression expr = exprFactory.list(
      exprFactory.literal("foo"),
      exprFactory.literal(1),
      exprFactory.literal(1.1),
      exprFactory.literal(true),
      exprFactory.symbol("bar")
    );
    JsonNode result = classUnderTest.toJson(expr);
    assertEquals(6, result.size());
    assertEquals("list", result.get(0).getTextValue());
    assertEquals("foo", result.get(1).getTextValue());
    assertEquals(1, result.get(2).getLongValue());
    assertEquals(1.1, result.get(3).getDoubleValue(), 0.00000001);
    assertEquals(true, result.get(4).getBooleanValue());
    assertEquals(2, result.get(5).size());
    assertEquals("symbol", result.get(5).get(0).getTextValue());
    assertEquals("bar", result.get(5).get(1).getTextValue());
  }

  @Test
  public void toJson_AttributeQuery() {
    AttributeQuery query = new AttributeQuery();
    query.fetch("foo", exprFactory.attribute("where/foo", "string"));
    query.setFilter(exprFactory.literal(true));
    query.appendOrderClause(
      exprFactory.desc(exprFactory.attribute("where/bar", "string"))
    );
    query.appendOrderClause(
      exprFactory.asc(exprFactory.attribute("where/qwe", "string"))
    );
    query.appendGroupClause(exprFactory.attribute("where/asd", "string"));
    query.appendGroupClause(exprFactory.attribute("where/qaz", "string"));
    query.setLimit(1);
    query.setSkip(1);

    JsonNode result = classUnderTest.toJson(query);
    assertTrue(result.has("fetch"));
    assertTrue(result.has("filter"));
    assertTrue(result.has("order"));
    assertTrue(result.has("group"));
    assertTrue(result.has("limit"));
    assertTrue(result.has("skip"));
    assertEquals(6, result.size());
  }

  @Test
  public void toJson_AttributeQuery_withDistinct() {
    AttributeQuery query = new AttributeQuery();
    
    JsonNode result = classUnderTest.toJson(query);
    assertFalse(result.has("distinct"));
    query.setDistinct(true);
    result = classUnderTest.toJson(query);
    assertTrue(result.has("distinct"));
  }
  
  @Test
  public void toJson_AttributeQuery_empty() {
    AttributeQuery query = new AttributeQuery();

    JsonNode result = classUnderTest.toJson(query);
    assertTrue(result.has("fetch"));
    assertEquals(1, result.size());
  }
  
  @Test
  public void toJson_Source() {
    Source source = new Source("nisse");
    source.put("ah", "ahvalue");
    source.put("oh", "ohvalue");
    
    JsonNode result = classUnderTest.toJson(source);
    JsonNode srcnode = result.get("source");
    assertEquals("nisse", srcnode.get("name").getTextValue());
    
    JsonNode values = srcnode.get("values");
    assertEquals("ahvalue", values.get("ah").getTextValue());
    assertEquals("ohvalue", values.get("oh").getTextValue());
  }
}
