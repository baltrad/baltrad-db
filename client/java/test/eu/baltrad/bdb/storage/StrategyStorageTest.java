package eu.baltrad.bdb.storage;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import org.easymock.EasyMockSupport;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import eu.baltrad.bdb.db.FileEntry;

import static org.easymock.EasyMock.*;
import static org.junit.Assert.assertSame;

public class StrategyStorageTest extends EasyMockSupport {
  private StrategyStorage classUnderTest = null;
  private LocalStorage dbStorage = null;
  private LocalStorage fsStorage = null;
  private Map<String, LocalStorage> storages = null;

  @Before
  public void setUp() {
    dbStorage = createMock(LocalStorage.class);
    fsStorage = createMock(LocalStorage.class);
    storages = new HashMap<String, LocalStorage>();
    storages.put("db", dbStorage);
    storages.put("fs", fsStorage);
    classUnderTest = new StrategyStorage();
    classUnderTest.setStrategies(storages);
    classUnderTest.setStrategy("db");
    classUnderTest.init();
  }
  
  @After
  public void tearDown() {
    dbStorage = null;
    fsStorage = null;
    storages = null;
    classUnderTest = null;
  }

  @Test
  public void test_store_db() {
    FileEntry entry = createMock(FileEntry.class);
    File f = new File("x");
    expect(dbStorage.store(entry)).andReturn(f);
    
    replayAll();
    
    File result = classUnderTest.store(entry);
    
    verifyAll();
    assertSame(result, f);
  }

  @Test
  public void test_store_entry_and_is_db() {
    FileEntry entry = createMock(FileEntry.class);
    InputStream content = new InputStream() {
      @Override
      public int read() throws IOException {
        return 0;
      }
    };
    File f = new File("x");
    expect(dbStorage.store(entry, content)).andReturn(f);
    
    replayAll();
    
    File result = classUnderTest.store(entry, content);
    
    verifyAll();
    assertSame(result, f);
  }
  
  @Test
  public void test_store_fs() {
    FileEntry entry = createMock(FileEntry.class);
    File f = new File("x");
    classUnderTest.setStrategy("fs");
    classUnderTest.init();
    
    expect(fsStorage.store(entry)).andReturn(f);
    
    replayAll();
    
    File result = classUnderTest.store(entry);
    
    verifyAll();
    assertSame(result, f);
  }

  @Test
  public void test_remove() {
    FileEntry entry = createMock(FileEntry.class);
    File f = new File("x");
    classUnderTest.setStrategy("fs");
    classUnderTest.init();
    
    fsStorage.remove(entry);
    
    replayAll();
    
    classUnderTest.remove(entry);
    
    verifyAll();
  }
}
