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

package eu.baltrad.bdb.oh5;

import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class MetadataMatcherTest {
  private Metadata metadata;
  private ExpressionFactory xpr;
  private MetadataMatcher classUnderTest;

  private void initMetadata() {
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "WMO:012345"));
    metadata.addNode("/what", new Attribute("date", "20111213"));
    metadata.addNode("/what", new Attribute("time", "141516"));
    metadata.addNode("/what", new Attribute("object", "PVOL"));
    metadata.addNode("/", new Group("dataset1"));
    metadata.addNode("/dataset1", new Group("where"));
    metadata.addNode("/dataset1/where", new Attribute("xsize", 1));
    metadata.addNode("/dataset1/where", new Attribute("ysize", 2));
    metadata.addNode("/", new Group("dataset2"));
    metadata.addNode("/dataset2", new Group("where"));
    metadata.addNode("/dataset2/where", new Attribute("xsize", 3));
    metadata.addNode("/dataset2/where", new Attribute("ysize", 4));
    metadata.addNode("/dataset2", new Group("data1"));
    metadata.addNode("/dataset2/data1", new Group("where"));
    metadata.addNode("/dataset2/data1/where", new Attribute("xsize", 5));
    metadata.addNode("/dataset2/data1/where", new Attribute("usize", 6));
  }
  
  @Before
  public void setUp() {
    metadata = new Metadata();
    initMetadata();
    xpr = new ExpressionFactory();
    classUnderTest = new MetadataMatcher();
  }

  @Test
  public void match_what_object_eq_pvol() {
    Expression expr = xpr.eq(
      xpr.attribute("what/object", "string"), xpr.literal("PVOL")
    );
    assertTrue(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_what_object_eq_scan() {
    Expression expr = xpr.eq(
      xpr.attribute("what/object", "string"), xpr.literal("SCAN")
    );
    assertFalse(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_what_object_in_scan_pvol() {
    Expression expr = xpr.in(
      xpr.attribute("what/object", "string"),
      xpr.list(xpr.literal("SCAN"), xpr.literal("PVOL"))
    );
    assertTrue(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_what_object_in_scan_comp() {
    Expression expr = xpr.in(
      xpr.attribute("what/object", "string"),
      xpr.list(xpr.literal("SCAN"), xpr.literal("COMP"))
    );
    assertFalse(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_what_sourceWMO_eq_012345() {
    Expression expr = xpr.eq(
      xpr.attribute("what/source:WMO", "string"),
      xpr.literal("012345")
    );
    assertTrue(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_what_sourceWMO_eq_54321() {
    Expression expr = xpr.eq(
      xpr.attribute("what/source:WMO", "string"),
      xpr.literal("54321")
    );
    assertFalse(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_where_xsize_lt_2_and_where_ysize_gt_2() {
    Expression expr = xpr.and(
      xpr.lt(
        xpr.attribute("where/xsize", "long"),
        xpr.literal(2)
      ),
      xpr.gt(
        xpr.attribute("where/ysize", "long"),
        xpr.literal(2)
      )
    );
    assertTrue(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_where_xsize_lt_1_and_where_ysize_gt_2() {
    Expression expr = xpr.and(
      xpr.lt(
        xpr.attribute("where/xsize", "long"),
        xpr.literal(1)
      ),
      xpr.gt(
        xpr.attribute("where/ysize", "long"),
        xpr.literal(2)
      )
    );
    assertFalse(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_where_xsize_lt_2_or_where_ysize_gt_2() {
    Expression expr = xpr.or(
      xpr.lt(
        xpr.attribute("where/xsize", "long"),
        xpr.literal(1)
      ),
      xpr.gt(
        xpr.attribute("where/ysize", "long"),
        xpr.literal(2)
      )
    );
    assertTrue(classUnderTest.match(metadata, expr));
  }

  @Test
  public void match_where_xsize_lt_2_or_where_ysize_gt_6() {
    Expression expr = xpr.or(
      xpr.lt(
        xpr.attribute("where/xsize", "long"),
        xpr.literal(1)
      ),
      xpr.gt(
        xpr.attribute("where/ysize", "long"),
        xpr.literal(6)
      )
    );
    assertFalse(classUnderTest.match(metadata, expr));
  }
  
  public static class MetadataMatcherThread extends Thread {
    private MetadataMatcher matcher;
    private Metadata metadata;
    private ExpressionFactory xpr;
    private boolean evaluationError = false;
    private boolean otherError = false;
    
    public MetadataMatcherThread(MetadataMatcher matcher, ExpressionFactory xpr, Metadata metadata) {
      this.matcher = matcher;
      this.xpr = xpr;
      this.metadata = metadata;
    }
    
    @Override
    public void run() {
      try {
        for (int i = 0; i < 100; i++) {
          Expression expr = xpr.eq(
              xpr.attribute("what/source:WMO", "string"),
              xpr.literal("012345")
            );
          matcher.match(this.metadata, expr);
        }
      } catch (EvaluationError e) {
        e.printStackTrace();
        evaluationError = true;
      } catch (Exception e) {
        e.printStackTrace();
        otherError = true;
      }
      synchronized (this) {
        notifyAll();
      }
    }
    
    public boolean isEvaluationError() {
      return evaluationError;
    }
    
    public boolean isOtherError() {
      return otherError;
    }
  };
  
  @Test
  public void match_several_threads() {
    MetadataMatcherThread[] threads = new MetadataMatcherThread[20];
    for (int i = 0; i < threads.length; i++) {
      threads[i] = new MetadataMatcherThread(classUnderTest, xpr, metadata);
      threads[i].start();
    }
    
    for (int i = 0; i < threads.length; i++) {
      try {
        threads[i].join(60000);
      } catch (InterruptedException e) {
        // pass
      }
    }
    
    for (int i = 0; i < threads.length; i++) {
      assertEquals(false, threads[i].isEvaluationError());
      assertEquals(false, threads[i].isOtherError());
    }
  }
}
