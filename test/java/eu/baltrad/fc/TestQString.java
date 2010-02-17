package eu.baltrad.fc;

import eu.baltrad.fc.TestSwig;

import junit.framework.TestCase;

public class TestQString extends TestCase {
  private TestSwig swig;
  private String str;

  public void setUp() {
    swig = new TestSwig();
    str = new String("\u00F6\u00E4\u00FC\u20AC");
  }

  public void test_qstring_to_string() {
    String qstr = swig.qstring_to_string(str);
    assertEquals(qstr, str);
  }

  public void test_qstringconstref_to_string() {
    String qstr = swig.qstringconstref_to_string(str);
    assertEquals(qstr, str);
  }

  public void test_string_to_qstring() {
    String qstr = swig.string_to_qstring(str);
    assertEquals(qstr, str);
  }

  public void test_string_to_qstringconstref() {
    String qstr = swig.string_to_qstringconstref(str);
    assertEquals(qstr, str);
  }

}
