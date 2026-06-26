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

package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.oh5.Source;

import org.apache.hc.core5.http.HttpEntity;
import org.apache.hc.core5.http.Header;
import org.apache.hc.client5.http.classic.methods.HttpUriRequestBase;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.net.URI;
import java.util.UUID;

public class DefaultRequestFactoryTest {
  DefaultRequestFactory classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com/")
    );
  }

  protected String getHeader(HttpUriRequestBase request, String name) {
    Header header = request.getFirstHeader(name);
    if (header != null) {
      return header.getValue();
    } else {
      return null;
    }
  }

  protected HttpEntity getEntity(HttpUriRequestBase req) {
    try {
      if (req instanceof org.apache.hc.core5.http.HttpEntityContainer) {
        return ((org.apache.hc.core5.http.HttpEntityContainer)req).getEntity();
      }
    } catch (Exception e) {
      return null;
    }
    return null;
  }

  protected String getContentType(HttpUriRequestBase req) {
    Header header = req.getFirstHeader("content-type");
    if (header != null)
      return header.getValue();
    return null;
  }

  @Test
  public void createStoreFileRequest() throws Exception {
    InputStream input = new ByteArrayInputStream("filecontent".getBytes());
    HttpUriRequestBase req = classUnderTest.createStoreFileRequest(input);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/file/"), req.getUri());
    assertEquals("application/x-hdf5", getContentType(req));
  }

  @Test
  public void createRemoveFileEntryRequest() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequestBase req = classUnderTest.createRemoveFileEntryRequest(uuid);

    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001"), req.getUri());
  }

  @Test
  public void createRemoveAllFileEntriesRequest() throws Exception {
    HttpUriRequestBase req = classUnderTest.createRemoveAllFileEntriesRequest();

    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/file/"), req.getUri());
  }

  @Test
  public void createGetFileEntryRequest() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequestBase req = classUnderTest.createGetFileEntryRequest(uuid);

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001/metadata"), req.getUri());
  }

  @Test
  public void createGetFileContentRequest() throws Exception {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequestBase req = classUnderTest.createGetFileContentRequest(uuid);

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001"), req.getUri());
  }

  @Test
  public void createQueryFileRequest() throws Exception {
    FileQuery query = new FileQuery();
    HttpUriRequestBase req = classUnderTest.createQueryFileRequest(query);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/query/file"), req.getUri());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }

  @Test
  public void createQueryAttributeRequest() throws Exception {
    AttributeQuery query = new AttributeQuery();
    HttpUriRequestBase req = classUnderTest.createQueryAttributeRequest(query);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/query/attribute"), req.getUri());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }

  @Test
  public void createGetSourcesRequest() throws Exception {
    HttpUriRequestBase req = classUnderTest.createGetSourcesRequest();

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getUri());
  }
  
  @Test
  public void createGetSourceRequest() throws Exception {
    HttpUriRequestBase req = classUnderTest.createGetSourceRequest("se");
    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/source/by_name/se"), req.getUri());
  }
  
  @Test
  public void createAddSourceRequest() throws Exception {
    Source source = new Source("mysource");
    source.put("oh", "ohvalue");
    source.put("ah", "ahvalue");
    
    HttpUriRequestBase req = classUnderTest.createAddSourceRequest(source);
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getUri());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void createUpdateSourceRequest() throws Exception {
    Source source = new Source("mysource");
    source.put("oh", "ohvalue");
    source.put("ah", "ahvalue");
    
    HttpUriRequestBase req = classUnderTest.createUpdateSourceRequest(source);
    assertEquals("PUT", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getUri());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void createDeleteSourceRequest() throws Exception {
    HttpUriRequestBase req = classUnderTest.createDeleteSourceRequest("nisse");
    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/source/nisse"), req.getUri());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void getRequestUri_serverWithoutSlash() throws Exception {
    classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080")
    );

    assertEquals(
      URI.create("http://example.com:8080/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }

  @Test
  public void getRequestUri_serverWithSlash() throws Exception {
    classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080/")
    );

    assertEquals(
      URI.create("http://example.com:8080/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }

  @Test
  public void getRequestUri_serverWithPath() throws Exception {
      classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080/path/")
    );

    assertEquals(
      URI.create("http://example.com:8080/path/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }
}
