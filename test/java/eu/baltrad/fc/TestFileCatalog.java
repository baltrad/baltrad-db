/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

import eu.baltrad.fc.FileCatalog;
import eu.baltrad.fc.Date;
import eu.baltrad.fc.Time;
import eu.baltrad.fc.Variant;
import eu.baltrad.fc.FileSystemError;

import eu.baltrad.fc.db.FileEntry;

import eu.baltrad.fc.oh5.hl.HlFile;

import eu.baltrad.fc.test.TestRDB;
import eu.baltrad.fc.test.TempH5File;
import eu.baltrad.fc.test.TempDir;

import junit.framework.TestCase;

public class TestFileCatalog extends TestCase {
  private TestRDB rdb;
  private String dsn;
  private String schema_dir;
  private FileCatalog fc;
  private HlFile file;
  private TempH5File tempfile;
  private TempDir tempdir;

  public void setUp() {
    dsn = System.getProperty("db.test_db_dsn");
    schema_dir = System.getProperty("db.test_db_schema_dir");
    tempdir = new TempDir();
    rdb = new TestRDB(dsn, schema_dir);
    fc = new FileCatalog(dsn);

    file = new HlFile("pvol",
                      new Date(2000, 05, 01),
                      new Time(12, 01, 05),
                      "RAD:SE50");
    tempfile = new TempH5File();
    tempfile.write(file);
  }

  public void tearDown() {
    tempfile.unlink();
    tempdir.unlink();
    rdb.clean();
  }

  public void testStore() {
    FileEntry f = fc.store(tempfile.path());
    assertTrue(f.id() > 0);
  }

  public void testStoreNXFile() {
    try {
      FileEntry f = fc.store"/path/to/nxfile");
      fail("expected FileSystemError");
    } catch (FileSystemError e) {
      // pass
    }
  }

  public void testCustomFileNamer() {
    FileNamer namer = new IncrementalFileNamer(1);
    assertEquals("1", namer.name(file));
    assertEquals("2", namer.name(file));
  }

}
