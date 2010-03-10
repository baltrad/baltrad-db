package eu.baltrad.fc;

import eu.baltrad.fc.FileCatalog;
import eu.baltrad.fc.Date;
import eu.baltrad.fc.Time;
import eu.baltrad.fc.Variant;
import eu.baltrad.fc.FileSystemError;

import eu.baltrad.fc.oh5.File;

import eu.baltrad.fc.test.TestRDB;
import eu.baltrad.fc.test.TempH5File;
import eu.baltrad.fc.test.TempDir;

import junit.framework.TestCase;

public class TestFileCatalog extends TestCase {
  private TestRDB rdb;
  private String dsn;
  private String schema_dir;
  private FileCatalog fc;
  private File file;
  private TempH5File tempfile;
  private TempDir tempdir;

  public void setUp() {
    dsn = System.getProperty("db.test_db_dsn");
    schema_dir = System.getProperty("db.test_db_schema_dir");
    tempdir = new TempDir();
    rdb = new TestRDB(dsn, schema_dir);
    fc = new FileCatalog(dsn, tempdir.path());

    file = File.minimal("pvol",
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

  public void testImport() {
    File f = fc.catalog(tempfile.filename());
    assertNotNull(f);
    assertTrue(f.db_id() > 0);
  }

  public void testImportNXFile() {
    try {
      File f = fc.catalog("/path/to/nxfile");
      fail("expected FileSystemError");
    } catch (FileSystemError e) {
      // pass
    }
  }

}
