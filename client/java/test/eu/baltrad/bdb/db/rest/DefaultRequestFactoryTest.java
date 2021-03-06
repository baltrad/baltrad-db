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

import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.Header;
import org.apache.http.client.methods.HttpUriRequest;

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

  protected String getHeader(HttpUriRequest request, String name) {
    Header header = request.getFirstHeader(name);
    if (header != null) {
      return header.getValue();
    } else {
      return null;
    }
  }

  protected HttpEntity getEntity(HttpUriRequest req) {
    HttpEntityEnclosingRequest entityReq = null;
    try {
      entityReq = (HttpEntityEnclosingRequest)req;
    } catch (ClassCastException e) {
      return null;
    }
    return entityReq.getEntity();
  }

  protected String getContentType(HttpUriRequest req) {
    Header header = req.getFirstHeader("content-type");
    if (header != null)
      return header.getValue();
    return null;
  }

  @Test
  public void createStoreFileRequest() {
    InputStream input = new ByteArrayInputStream("filecontent".getBytes());
    HttpUriRequest req = classUnderTest.createStoreFileRequest(input);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/file/"), req.getURI());
    assertEquals("application/x-hdf5", getContentType(req));
  }

  @Test
  public void createRemoveFileEntryRequest() {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequest req = classUnderTest.createRemoveFileEntryRequest(uuid);

    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001"), req.getURI());
  }

  @Test
  public void createRemoveAllFileEntriesRequest() {
    HttpUriRequest req = classUnderTest.createRemoveAllFileEntriesRequest();

    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/file/"), req.getURI());
  }

  @Test
  public void createGetFileEntryRequest() {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequest req = classUnderTest.createGetFileEntryRequest(uuid);

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001/metadata"), req.getURI());
  }

  @Test
  public void createGetFileContentRequest() {
    UUID uuid = UUID.fromString("00000000-0000-0000-0004-000000000001");
    HttpUriRequest req = classUnderTest.createGetFileContentRequest(uuid);

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/file/00000000-0000-0000-0004-000000000001"), req.getURI());
  }

  @Test
  public void createQueryFileRequest() {
    FileQuery query = new FileQuery();
    HttpUriRequest req = classUnderTest.createQueryFileRequest(query);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/query/file"), req.getURI());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }

  @Test
  public void createQueryAttributeRequest() {
    AttributeQuery query = new AttributeQuery();
    HttpUriRequest req = classUnderTest.createQueryAttributeRequest(query);
    
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/query/attribute"), req.getURI());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }

  @Test
  public void createGetSourcesRequest() {
    HttpUriRequest req = classUnderTest.createGetSourcesRequest();

    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getURI());
  }
  
  @Test
  public void createGetSourceRequest() {
    HttpUriRequest req = classUnderTest.createGetSourceRequest("se");
    assertEquals("GET", req.getMethod());
    assertEquals(URI.create("http://example.com/source/by_name/se"), req.getURI());
  }
  
  @Test
  public void createAddSourceRequest() {
    Source source = new Source("mysource");
    source.put("oh", "ohvalue");
    source.put("ah", "ahvalue");
    
    HttpUriRequest req = classUnderTest.createAddSourceRequest(source);
    assertEquals("POST", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getURI());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void createUpdateSourceRequest() {
    Source source = new Source("mysource");
    source.put("oh", "ohvalue");
    source.put("ah", "ahvalue");
    
    HttpUriRequest req = classUnderTest.createUpdateSourceRequest(source);
    assertEquals("PUT", req.getMethod());
    assertEquals(URI.create("http://example.com/source/"), req.getURI());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void createDeleteSourceRequest() {
    HttpUriRequest req = classUnderTest.createDeleteSourceRequest("nisse");
    assertEquals("DELETE", req.getMethod());
    assertEquals(URI.create("http://example.com/source/nisse"), req.getURI());
    assertEquals("application/json; charset=utf-8", getContentType(req));
  }
  
  @Test
  public void getRequestUri_serverWithoutSlash() {
    classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080")
    );

    assertEquals(
      URI.create("http://example.com:8080/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }

  @Test
  public void getRequestUri_serverWithSlash() {
    classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080/")
    );

    assertEquals(
      URI.create("http://example.com:8080/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }

  @Test
  public void getRequestUri_serverWithPath() {
      classUnderTest = new DefaultRequestFactory(
      URI.create("http://example.com:8080/path/")
    );

    assertEquals(
      URI.create("http://example.com:8080/path/bla"),
      classUnderTest.getRequestUri("bla")
    );
  }
}
