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

import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;

import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.node.ArrayNode;
import org.codehaus.jackson.node.JsonNodeFactory;
import org.codehaus.jackson.node.ObjectNode;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

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
  public void toJson_Expression() {
    Expression expr = exprFactory.list(
      exprFactory.literal("foo"),
      exprFactory.literal(1),
      exprFactory.literal(1.1),
      exprFactory.literal(true),
      exprFactory.symbol("bar")
    );
    JsonNode result = classUnderTest.toJson(expr);
    assertEquals(5, result.size());
    assertEquals("foo", result.get(0).getTextValue());
    assertEquals(1, result.get(1).getLongValue());
    assertEquals(1.1, result.get(2).getDoubleValue(), 0.00000001);
    assertEquals(true, result.get(3).getBooleanValue());
    assertEquals(2, result.get(4).size());
    assertEquals("symbol", result.get(4).get(0).getTextValue());
    assertEquals("bar", result.get(4).get(1).getTextValue());
  }
}
