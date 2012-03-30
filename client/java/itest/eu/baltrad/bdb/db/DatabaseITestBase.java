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

    assertNotNull(seang);
    assertEquals("Ängelholm", seang.get("PLC"));
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
