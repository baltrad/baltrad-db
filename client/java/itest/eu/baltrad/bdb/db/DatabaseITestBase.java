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

package eu.baltrad.bdb.db;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;
import static org.junit.Assume.assumeNotNull;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.util.List;
import java.util.UUID;

import org.apache.commons.codec.binary.Hex;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import eu.baltrad.bdb.expr.ExpressionFactory;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;


public abstract class DatabaseITestBase {
  protected Database classUnderTest;

  protected abstract Database createDatabase();

  private String getFilePath(String resource) throws Exception {
    File f = new File(DatabaseITestBase.class.getResource(resource).getFile());
    return f.getAbsolutePath();
  }

  @Before
  public void setUp() {
    classUnderTest = createDatabase();
    assumeNotNull(classUnderTest);
    classUnderTest.removeAllFileEntries();
    try {
      classUnderTest.getSourceManager().remove("newsourcename");
    } catch (Exception e) {
      
    }
  }

  @After
  public void tearDown() {
  }

  @Test
  public void store_seang_scan() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry result = classUnderTest.store(input);
    assertNotNull(result);
    assertNotNull(result.getUuid());
  }

  @Test
  public void store_minsk_scan() throws Exception {
    // this file contains ODIM_H5/2.1 attributes with array values
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/minsk_20120118_081300_Elevation9.h5")
    );
    FileEntry result = classUnderTest.store(input);
    assertNotNull(result);
    assertNotNull(result.getUuid());
  }

  @Test
  public void store_duplicate() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry result = classUnderTest.store(input);
    input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    try {
      classUnderTest.store(input);
      fail("expected DuplicateEntry");
    } catch (DuplicateEntry e) { }
  }

  @Test
  public void getFileEntry() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry storedEntry = classUnderTest.store(input);
    
    FileEntry result = classUnderTest.getFileEntry(storedEntry.getUuid());
    assertNotNull(result);
  }

  @Test
  public void removeFilesByCount() throws Exception {
    FileEntry storedEntry1 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080000_searl_000000.h5")));
    FileEntry storedEntry2 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080000_sease_000000.h5")));
    FileEntry storedEntry3 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080500_seang_000000.h5")));
    FileEntry storedEntry4 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")));
    
    assertEquals(4, classUnderTest.getFileCount());
    
    int result = classUnderTest.removeFilesByCount(3,2);
    assertEquals(1, result);
    assertEquals(3, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByCount(1,2);
    assertEquals(2, result);
    assertEquals(1, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByCount(0,100);
    assertEquals(1, result);
    assertEquals(0, classUnderTest.getFileCount());
  }

  @Test
  public void removeFilesByAge() throws Exception {
    FileEntry storedEntry1 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080000_searl_000000.h5")));
    FileEntry storedEntry2 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080000_sease_000000.h5")));
    FileEntry storedEntry3 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101016080500_seang_000000.h5")));
    FileEntry storedEntry4 = classUnderTest.store(new FileInputStream(
        getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")));
    
    assertEquals(4, classUnderTest.getFileCount());
    
    int result = classUnderTest.removeFilesByAge(new DateTime(2010,10,16,8,5,0),2);
    assertEquals(2, result);
    assertEquals(2, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByAge(new DateTime(2010,10,17,0,0,0),2);
    assertEquals(1, result);
    assertEquals(1, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByAge(new DateTime(2010,10,23,17,59,59),2);
    assertEquals(0, result);
    assertEquals(1, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByAge(new DateTime(2010,10,23,18,0,0),2);
    assertEquals(0, result);
    assertEquals(1, classUnderTest.getFileCount());

    result = classUnderTest.removeFilesByAge(new DateTime(2010,10,23,18,0,1),2);
    assertEquals(1, result);
    assertEquals(0, classUnderTest.getFileCount());
  }

  @Test
  public void getFileCount() throws Exception {
    assertEquals(0, classUnderTest.getFileCount());
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    classUnderTest.store(input);
    assertEquals(1, classUnderTest.getFileCount());
  }

  @Test
  public void getFileEntry_missing() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    FileEntry result = classUnderTest.getFileEntry(uuid);
    assertNull(result);
  }

  @Test
  public void removeFile() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry storedEntry = classUnderTest.store(input);

    assertTrue(classUnderTest.removeFileEntry(storedEntry.getUuid()));

    FileEntry result = classUnderTest.getFileEntry(storedEntry.getUuid());
    assertNull(result);
  }

  @Test
  public void removeFile_missing() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    assertFalse(classUnderTest.removeFileEntry(uuid));
  }

  @Test
  public void getFileContent() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry storedEntry = classUnderTest.store(input);

    InputStream result = classUnderTest.getFileContent(storedEntry.getUuid());
    DigestInputStream digestStream = new DigestInputStream(
      result,
      MessageDigest.getInstance("MD5")
    );
    while (digestStream.read() != -1) { }
    result.close();
    assertEquals(
      "94a52469044db49186b2a00fee5d8426",
      Hex.encodeHexString(digestStream.getMessageDigest().digest())
    );
  }

  @Test
  public void getFileContent_missing() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    assertNull(classUnderTest.getFileContent(uuid));
  }

  @Test
  public void execute_FileQuery() throws Exception {
    FileInputStream input = new FileInputStream(
      getFilePath("fixtures/Z_SCAN_C_ESWI_20101023180000_seang_000000.h5")
    );
    FileEntry storedEntry = classUnderTest.store(input);

    FileQuery query = new FileQuery();
    ExpressionFactory xpr = new ExpressionFactory();
    query.setFilter(
      xpr.and(
        xpr.eq(
          xpr.literal("SCAN"),
          xpr.attribute("what/object", "string")
        ),
        xpr.eq(
          xpr.literal("seang"),
          xpr.attribute("_bdb/source:NOD", "string")
        )
      )
    );

    FileResult result = classUnderTest.execute(query);
    assertTrue(result.next());
    FileEntry queriedEntry = result.getFileEntry();
    assertNotNull(queriedEntry);
    assertEquals(storedEntry.getUuid(), queriedEntry.getUuid());
  }

  @Test
  public void getSources() throws Exception {
    List<Source> sources = classUnderTest.getSources();
    
    // find seang
    Source seang = null;
    for (Source src : sources) {
      if ("seang".equals(src.getName())) {
        seang = src;
        break;
      }
    }
  }

  @Test
  public void SourceManager_getSources() throws Exception {
    List<Source> sources = classUnderTest.getSourceManager().getSources();
    // find seang
    Source seang = null;
    for (Source src : sources) {
      if ("seang".equals(src.getName())) {
        seang = src;
        break;
      }
    }
    
    assertNotNull(seang);
    assertEquals("Ängelholm", seang.get("PLC"));
  }

  @Test
  public void SourceManager_getSource_seang() throws Exception {
    Source seang = classUnderTest.getSourceManager().getSource("seang");
    assertNotNull(seang);
    assertEquals("seang", seang.getName());
    assertEquals("Ängelholm", seang.get("PLC"));
    assertEquals("SE50", seang.get("RAD"));
    assertEquals("02606", seang.get("WMO"));
    assertEquals("seang", seang.get("NOD"));
    assertEquals("se", seang.getParent());
  }

  @Test
  public void SourceManager_getSource_se() throws Exception {
    Source se = classUnderTest.getSourceManager().getSource("se");
    assertNotNull(se);
    assertEquals("se", se.getName());
    assertEquals("ESWI", se.get("CCCC"));
    assertEquals("82", se.get("ORG"));
    assertEquals("643", se.get("CTY"));
    assertEquals(null, se.getParent());
  }

  @Test
  public void SourceManager_add() throws Exception {
    Source newsource = new Source("newsourcename");
    newsource.put("PLC", "New source town");
    newsource.put("CMT", "New comment field");
    
    classUnderTest.getSourceManager().add(newsource);
    
    List<Source> sources = classUnderTest.getSourceManager().getSources();

    // find source and place
    Source found = null;
    for (Source src : sources) {
      if ("newsourcename".equals(src.getName())) {
        found = src;
        break;
      }
    }
    
    assertNotNull(found);
    assertEquals("New source town", found.get("PLC"));
    assertEquals("New comment field", found.get("CMT"));
  }

  @Test
  public void SourceManager_update() throws Exception {
    Source newsource = new Source("newsourcename");
    newsource.put("PLC", "New source town");
    newsource.put("CMT", "New comment field");
    
    classUnderTest.getSourceManager().add(newsource);
    
    newsource.put("PLC", "Changed source town");
    
    classUnderTest.getSourceManager().update(newsource);
    
    List<Source> sources = classUnderTest.getSourceManager().getSources();

    // find source and place
    Source found = null;
    for (Source src : sources) {
      if ("newsourcename".equals(src.getName())) {
        found = src;
        break;
      }
    }
    
    assertNotNull(found);
    assertEquals("Changed source town", found.get("PLC"));
    assertEquals("New comment field", found.get("CMT"));
  }
  
  @Test
  public void SourceManager_updateNotExisting() throws Exception {
    Source newsource = new Source("newsourcename");
    newsource.put("PLC", "New source town");
    newsource.put("CMT", "New comment field");
    
    try {
      classUnderTest.getSourceManager().update(newsource);
      fail("Expected DatabaseError");
    } catch (DatabaseError e) {
      // pass
    }
    
    List<Source> sources = classUnderTest.getSourceManager().getSources();

    // find source and place
    Source found = null;
    for (Source src : sources) {
      if ("newsourcename".equals(src.getName())) {
        found = src;
        break;
      }
    }
    
    assertNull(found);
  }
  
  @Test
  public void SourceManager_remove() throws Exception {
    Source newsource = new Source("newsourcename");
    newsource.put("PLC", "New source town");
    newsource.put("CMT", "New comment field");

    classUnderTest.getSourceManager().add(newsource);
    classUnderTest.getSourceManager().remove("newsourcename");
    
    List<Source> sources = classUnderTest.getSourceManager().getSources();

    // find source and place
    Source found = null;
    for (Source src : sources) {
      if ("newsourcename".equals(src.getName())) {
        found = src;
        break;
      }
    }
    
    assertNull(found);
  }

  @Test
  public void SourceManager_getParentSources() throws Exception {
    List<Source> sources = classUnderTest.getSourceManager().getParentSources();
    Source by = null;
    Source se = null;
    Source pl = null;
    
    for (Source src : sources) {
      if ("by".equals(src.getName())) {
        by = src;
      } else if("se".equals(src.getName())) {
        se = src;
      } else if("pl".equals(src.getName())) {
        pl = src;
      }
    }
    assertEquals(3, sources.size());
    assertNotNull(by);
    assertNotNull(se);
    assertNotNull(pl);
    assertEquals("UMMS", by.get("CCCC"));
    assertEquals("226", by.get("ORG"));
    assertNull(by.getParent());
    assertEquals("ESWI", se.get("CCCC"));
    assertEquals("82", se.get("ORG"));
    assertEquals("643", se.get("CTY"));
    assertNull(se.getParent());
    assertEquals("SOWR", pl.get("CCCC"));
    assertEquals("220", pl.get("ORG"));
    assertNull(pl.getParent());
  }

  @Test
  public void SourceManager_getSourcesWithParent() throws Exception {
    List<Source> sources = classUnderTest.getSourceManager().getSourcesWithParent("se");
    Source selul = null;
    Source searl = null;
    
    for (Source src : sources) {
      if ("selul".equals(src.getName())) {
        selul = src;
      } else if("searl".equals(src.getName())) {
        searl = src;
      }
    }
    assertEquals(12, sources.size());
    assertNotNull(selul);
    assertNotNull(searl);
    //<selul nod="selul" plc="Luleå" rad="SE41" wmo="02092"></selul>
    assertEquals("selul", selul.get("NOD"));
    assertEquals("Luleå", selul.get("PLC"));
    assertEquals("SE41", selul.get("RAD"));
    assertEquals("02092", selul.get("WMO"));
    assertEquals("se", selul.getParent());
    //<searl nod="searl" plc="Arlanda" rad="SE46" wmo="02451"></searl>
    assertEquals("searl", searl.get("NOD"));
    assertEquals("Arlanda", searl.get("PLC"));
    assertEquals("SE46", searl.get("RAD"));
    assertEquals("02451", searl.get("WMO"));
    assertEquals("se", searl.getParent());
  }
  
  @Test
  public void findByDiadristicName() throws Exception {
    FileInputStream input = new FileInputStream(
        getFilePath("fixtures/gdansk.h5")
      );
    classUnderTest.store(input);
    AttributeQuery q = new AttributeQuery();
    ExpressionFactory xpr = new ExpressionFactory();
    q.fetch("what_source", xpr.attribute("_bdb/source:WMO"));
    q.setFilter(xpr.eq(xpr.attribute("_bdb/source:PLC"),xpr.literal("Gdańsk")));
    AttributeResult rs = classUnderTest.execute(q);
    assertEquals(1, rs.size());
  }

}
