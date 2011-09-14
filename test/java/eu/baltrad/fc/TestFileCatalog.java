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

package eu.baltrad.fc;

import java.io.File;
import java.util.List;

import eu.baltrad.fc.Date;
import eu.baltrad.fc.FileCatalog;
import eu.baltrad.fc.FileSystemError;
import eu.baltrad.fc.NullStorage;
import eu.baltrad.fc.Time;
import eu.baltrad.fc.Variant;

import eu.baltrad.fc.Database;
import eu.baltrad.fc.FileEntry;
import eu.baltrad.fc.FileQuery;

import eu.baltrad.fc.Oh5File;
import eu.baltrad.fc.Oh5HlFileWriter;
import eu.baltrad.fc.Oh5PhysicalFile;
import eu.baltrad.fc.Oh5Source;

import eu.baltrad.fc.TestRDB;

import junit.framework.TestCase;

public class TestFileCatalog extends TestCase {
  private TestRDB rdb;
  private String dsn;
  private String schema_dir;
  private FileCatalog fc;
  private Oh5PhysicalFile file;

  public void setUp() {
    dsn = System.getProperty("db.test_db_dsn");
    schema_dir = System.getProperty("db.test_db_schema_dir");
    rdb = new TestRDB(dsn, schema_dir);
    fc = new FileCatalog(Database.create(dsn), new NullStorage());

    file = new Oh5PhysicalFile(
      "pvol", new Date(2000, 05, 01), new Time(12, 01, 05), "RAD:SE50"
    );
  }

  public void tearDown() {
    rdb.clean();
  }

  public void testStore() throws Exception {
    File tempfile = File.createTempFile("bdbtest", ".h5");
    Oh5HlFileWriter writer = new Oh5HlFileWriter();
    writer.write(file, tempfile.getAbsolutePath());
    FileEntry f = fc.store(tempfile.getAbsolutePath());
    assertTrue(f.uuid() != "");
  }

  public void testStoreNXFile() {
    try {
      FileEntry f = fc.store("/path/to/nxfile");
      fail("expected FileSystemError");
    } catch (FileSystemError e) {
      // pass
    }
  }

  public void testCustomFileNamer() {
    FileNamer namer = new FileNamer() {
      protected String do_name(Oh5File file) {
        return "file";
      }
    };

    assertEquals("file", namer.name(file));
  }

  public void testSourceList() {
    List<Oh5Source> sources = fc.database().sources();
    assertTrue(sources.size() > 0);
  }

  public void testFileQuery_default_limit() {
    FileQuery q = new FileQuery();
    assertEquals(0, q.limit());
  }
}
