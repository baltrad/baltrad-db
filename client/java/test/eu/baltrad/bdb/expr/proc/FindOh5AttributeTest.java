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

import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.ListExpression;
import eu.baltrad.bdb.expr.LongExpression;
import eu.baltrad.bdb.expr.StringExpression;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Group;
import eu.baltrad.bdb.oh5.Attribute;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class FindOh5AttributeTest {
  private FindOh5Attribute classUnderTest;
  private Metadata metadata;

  private void initMetadata() {
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "WMO:12345"));
    metadata.addNode("/", new Group("where"));
    metadata.addNode("/where", new Attribute("xsize", 1));
    metadata.addNode("/where", new Attribute("ysize", 2));
    metadata.addNode("/", new Group("dataset1"));
    metadata.addNode("/dataset1", new Group("where"));
    metadata.addNode("/dataset1/where", new Attribute("xsize", 3));
    metadata.addNode("/dataset1/where", new Attribute("ysize", 4));
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("source", "WMO:54321"));
  }

  @Before
  public void setUp() {
    metadata = new Metadata();
    initMetadata();
    classUnderTest = new FindOh5Attribute(metadata);
  }

  @Test
  public void findAll_xsize() {
    ListExpression result = classUnderTest.findAll("xsize", "long");
    assertEquals(2, result.size());
    assertEquals(Long.valueOf(1), result.get(0).toLong());
    assertEquals(Long.valueOf(3), result.get(1).toLong());
  }

  @Test
  public void findAll_size() {
    ListExpression result = classUnderTest.findAll("size", "long");
    assertEquals(0, result.size());
  }

  @Test
  public void findAll_dataset1_where_xsize() {
    ListExpression result = classUnderTest.findAll("dataset1/where/xsize", "long");
    assertEquals(1, result.size());
    assertEquals(Long.valueOf(3), result.get(0).toLong());
  }

  @Test
  public void findAll_what_sourceWMO() {
    ListExpression result = classUnderTest.findAll("what/source:WMO", "string");
    assertEquals(1, result.size());
    assertEquals("12345", result.get(0).toString());
  }

  @Test
  public void findAll_what_sourceNOD() {
    ListExpression result = classUnderTest.findAll("what/source:NOD", "string");
    assertEquals(0, result.size());
  }

  @Test
  public void findAll_bdb_sourceWMO() {
    ListExpression result = classUnderTest.findAll("_bdb/source:WMO", "string");
    assertEquals(1, result.size());
    assertEquals("54321", result.get(0).toString());
  }

  @Test
  public void findAll_bdb_sourceNOD() {
    ListExpression result = classUnderTest.findAll("_bdb/source:NOD", "string");
    assertEquals(0, result.size());
  }

  @Test
  public void execute_invalidNumberOfArguments() {
    ListExpression args = new ListExpression();
    try {
      classUnderTest.execute(args);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }

  @Test
  public void execute_invalidNameArgument() {
    ListExpression args = new ListExpression();
    args.add(new LongExpression(1));
    args.add(new StringExpression("string"));
    try {
      classUnderTest.execute(args);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }    
  }

  @Test
  public void execute_invalidTypeArgument() {
    ListExpression args = new ListExpression();
    args.add(new StringExpression("what/object"));
    args.add(new LongExpression(1));
    try {
      classUnderTest.execute(args);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }
}
