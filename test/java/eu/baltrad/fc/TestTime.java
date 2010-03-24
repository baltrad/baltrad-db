package eu.baltrad.fc;

import eu.baltrad.fc.Date;

import junit.framework.TestCase;

public class TestTime extends TestCase {

  public void test_to_string() {
    Time t = new Time(12, 00, 01);
    assertEquals(t.to_string("hhmmss"), "120001");
  }
}
