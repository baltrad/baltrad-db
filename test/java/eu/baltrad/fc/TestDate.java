package eu.baltrad.fc;

import eu.baltrad.fc.Date;

import junit.framework.TestCase;

public class TestDate extends TestCase {

  public void test_to_string() {
    Date d = new Date(2000, 05, 01);
    assertEquals(d.to_string("yyyyMMdd"), "20000501");
  }
}
