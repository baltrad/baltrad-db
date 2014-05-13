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

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.util.*;

public class TemplateMetadataNamerTest {
  private Metadata metadata;
  private TemplateMetadataNamer classUnderTest;

  @Before
  public void setup() {
    metadata = new Metadata();
  }

  @Test
  public void name_object_date_time() {
    classUnderTest = new TemplateMetadataNamer(
      "${/what/object}_${/what/date}T${/what/time}Z.h5"
    );
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("object", "SCAN"));
    metadata.addNode("/what", new Attribute("date", "01022012"));
    metadata.addNode("/what", new Attribute("time", "144600"));

    assertEquals("SCAN_01022012T144600Z.h5", classUnderTest.name(metadata));
  }

  @Test
  public void name_escapedPlaceholder() {
    classUnderTest = new TemplateMetadataNamer("$${/what/object}.h5");

    assertEquals("$${/what/object}.h5", classUnderTest.name(metadata));
  }

  @Test
  public void name_noPlaceholder() {
    classUnderTest = new TemplateMetadataNamer("{/what/object}.h5");

    assertEquals("{/what/object}.h5", classUnderTest.name(metadata));
  }

  @Test
  public void name_integerAttribute() {
    classUnderTest = new TemplateMetadataNamer("${/int}");
    metadata.addNode("/", new Attribute("int", 123));

    assertEquals("123", classUnderTest.name(metadata));
  }

  @Test
  public void name_doubleAttribute() {
    classUnderTest = new TemplateMetadataNamer("${/double}");
    metadata.addNode("/", new Attribute("double", 1.23));

    assertEquals("1.23", classUnderTest.name(metadata));
  }

  @Test
  public void name_missingAttribute() {
    classUnderTest = new TemplateMetadataNamer("${/missing/attribute}");

    assertEquals("null", classUnderTest.name(metadata));
  }

  @Test
  public void name_sourceCMT() {
    classUnderTest = new TemplateMetadataNamer("${_bdb/source:CMT}");
    
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("ick", classUnderTest.name(metadata));
  }

  @Test
  public void name_sourceNOD() {
    classUnderTest = new TemplateMetadataNamer("${_bdb/source:NOD}");
    
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("abc", classUnderTest.name(metadata));
  }

  @Test
  public void name_sourceORG_null() {
    classUnderTest = new TemplateMetadataNamer("${_bdb/source:ORG}");
    
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("null", classUnderTest.name(metadata));
  }

  @Test
  public void name_whatCMT() {
    classUnderTest = new TemplateMetadataNamer("${what/source:CMT}");
    
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("ick", classUnderTest.name(metadata));
  }

  @Test
  public void name_whatNOD() {
    classUnderTest = new TemplateMetadataNamer("${what/source:NOD}");
    
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("abc", classUnderTest.name(metadata));
  }

  @Test
  public void name_whatSlashNOD() {
    classUnderTest = new TemplateMetadataNamer("${/what/source:NOD}");
    
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("abc", classUnderTest.name(metadata));
  }

  @Test
  public void name_whatORG_null() {
    classUnderTest = new TemplateMetadataNamer("${what/source:ORG}");
    
    metadata.addNode("/", new Group("what"));
    metadata.addNode("/what", new Attribute("source", "NOD:abc,CMT:ick"));
    assertEquals("null", classUnderTest.name(metadata));
  }
}
