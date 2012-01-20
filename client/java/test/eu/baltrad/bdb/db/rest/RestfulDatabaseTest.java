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
import eu.baltrad.bdb.db.AttributeResult;
import eu.baltrad.bdb.db.DatabaseError;
import eu.baltrad.bdb.db.DuplicateEntry;
import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.db.FileResult;
import eu.baltrad.bdb.oh5.Source;

import org.apache.commons.io.IOUtils;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.ProtocolVersion;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.entity.StringEntity;
import org.apache.http.message.BasicHttpResponse;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.*;

public class RestfulDatabaseTest extends EasyMockSupport {
  private static interface RestfulDatabaseMethods {
    public RestfulResponse executeRequest(HttpUriRequest request);
  }

  RequestFactory requestFactory;
  HttpClient httpClient;
  Authenticator authenticator;
  RestfulDatabaseMethods methods;
  RestfulDatabase classUnderTest;
  
  HttpResponse createResponse(int code) throws Exception {
    return createResponse(code, null);
  }


  HttpResponse createResponse(int code, String content) throws Exception {
    HttpResponse response =  new BasicHttpResponse(
      new ProtocolVersion("HTTP", 1, 1), code, ""
    );
    if (content != null) {
      response.setEntity(new StringEntity(content));
    }
    return response;
  }

  RestfulResponse createRestfulResponse(int code) throws Exception {
    return createRestfulResponse(code, null);
  }

  RestfulResponse createRestfulResponse(int code, String content)
      throws Exception {
    return new RestfulResponse(createResponse(code, content));
  }

  @Before
  public void setUp() {
    requestFactory = createMock(RequestFactory.class);
    httpClient = createMock(HttpClient.class);
    authenticator = createMock(Authenticator.class);
    methods = createMock(RestfulDatabaseMethods.class);

    classUnderTest = new RestfulDatabase(
        requestFactory,
        httpClient,
        authenticator) {
      @Override
      protected RestfulResponse executeRequest(HttpUriRequest request) {
        return methods.executeRequest(request);
      }
    };
      
    resetAll();
  }

  @Test
  public void store() throws Exception {
    InputStream fileContent = createMock(InputStream.class);
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_CREATED,
      "{ \"metadata\" : [" +
           "{\"path\": \"/\", \"type\": \"group\"}" +
      "]}"
    );
    
    expect(requestFactory.createStoreFileRequest(fileContent))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();
    
    FileEntry entry = classUnderTest.store(fileContent);
    verifyAll();
  }

  @Test
  public void store_duplicateEntry() throws Exception {
    InputStream fileContent = createMock(InputStream.class);
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_CONFLICT);
    
    expect(requestFactory.createStoreFileRequest(fileContent))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();
    
    try {
      classUnderTest.store(fileContent);
      fail("expected DuplicateEntry");
    } catch (DuplicateEntry e) { }
    verifyAll();
  }

  @Test
  public void store_serverFailure() throws Exception {
    InputStream fileContent = createMock(InputStream.class);
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );
    
    expect(requestFactory.createStoreFileRequest(fileContent))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();
    
    try {
      classUnderTest.store(fileContent);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void removeFileEntry() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_NO_CONTENT);

    expect(requestFactory.createRemoveFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    assertTrue(classUnderTest.removeFileEntry(uuid));
    verifyAll();
  }

  @Test
  public void removeFileEntry_notFound() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_NOT_FOUND);

    expect(requestFactory.createRemoveFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    assertFalse(classUnderTest.removeFileEntry(uuid));
    verifyAll();
  }

  @Test
  public void removeFileEntry_serverFailure() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createRemoveFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.removeFileEntry(uuid);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void removeAllFileEntries() throws Exception {
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_NO_CONTENT);

    expect(requestFactory.createRemoveAllFileEntriesRequest())
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    classUnderTest.removeAllFileEntries();
    verifyAll();
  }

  @Test
  public void removeAllFileEntries_serverFailure() throws Exception {
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createRemoveAllFileEntriesRequest())
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.removeAllFileEntries();
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();

  }


  @Test
  public void getFileEntry() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_OK,
      "{ \"metadata\" : [" +
           "{\"path\": \"/\", \"type\": \"group\"}" +
      "]}"
    );

    expect(requestFactory.createGetFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    FileEntry entry = classUnderTest.getFileEntry(uuid);
    assertNotNull(entry);
    verifyAll();
  }

  @Test
  public void getFileEntry_notFound() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_NOT_FOUND);

    expect(requestFactory.createGetFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    FileEntry entry = classUnderTest.getFileEntry(uuid);
    assertNull(entry);
    verifyAll();
  }

  @Test
  public void getFileEntry_serverFailure() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createGetFileEntryRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.getFileEntry(uuid);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void getFileContent() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_OK,
      "filecontent"
    );

    expect(requestFactory.createGetFileContentRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    InputStream fileContent = classUnderTest.getFileContent(uuid);
    assertNotNull(fileContent);
    verifyAll();
  }

  @Test
  public void getFileContent_notFound() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(HttpStatus.SC_NOT_FOUND);

    expect(requestFactory.createGetFileContentRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    InputStream fileContent = classUnderTest.getFileContent(uuid);
    assertNull(fileContent);
    verifyAll();
  }

  @Test
  public void getFileContent_serverFailure() throws Exception {
    UUID uuid = UUID.randomUUID();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createGetFileContentRequest(uuid))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.getFileContent(uuid);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void execute_FileQuery() throws Exception {
    FileQuery query = new FileQuery();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_OK,
      "{ \"rows\" : []}"
    );

    expect(requestFactory.createQueryFileRequest(query))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    FileResult result = classUnderTest.execute(query);
    verifyAll();
  }

  @Test
  public void execute_FileQuery_serverFailure() throws Exception {
    FileQuery query = new FileQuery();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createQueryFileRequest(query))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.execute(query);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void execute_AttributeQuery() throws Exception {
    AttributeQuery query = new AttributeQuery();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_OK,
      "{\"rows\" : []}"
    );

    expect(requestFactory.createQueryAttributeRequest(query))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    AttributeResult result = classUnderTest.execute(query);
    verifyAll();
  }

  @Test
  public void execute_AttriuteQuery_serverFailure() throws Exception {
    AttributeQuery query = new AttributeQuery();
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createQueryAttributeRequest(query))
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();

    try {
      classUnderTest.execute(query);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void getSources() throws Exception {
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_OK,
      "{\"sources\" : [" +
        "{\"name\": \"src1\", \"values\": {\"key1\": \"value1\"}}," +
        "{\"name\": \"src2\", \"values\": {\"key2\": \"value2\"}}" +
      "]}"
    );

    expect(requestFactory.createGetSourcesRequest())
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();
    
    List<Source> sources = classUnderTest.getSources();
    assertEquals(2, sources.size());
    Source src = sources.get(0);
    assertEquals("src1", src.getName());
    assertEquals("value1", src.get("key1"));
    src = sources.get(1);
    assertEquals("src2", src.getName());
    assertEquals("value2", src.get("key2"));
    verifyAll();
  }

  @Test
  public void getSources_serverFailure() throws Exception {
    HttpUriRequest request = createMock(HttpUriRequest.class);
    RestfulResponse response = createRestfulResponse(
      HttpStatus.SC_INTERNAL_SERVER_ERROR
    );

    expect(requestFactory.createGetSourcesRequest())
      .andReturn(request);
    expect(methods.executeRequest(request))
      .andReturn(response);
    replayAll();
    
    try {
      classUnderTest.getSources();
      fail("expected DatabaseError");
    } catch (DatabaseError e) { }
    verifyAll();
  }

  @Test
  public void executeRequest() throws Exception {
    classUnderTest = new RestfulDatabase(
      requestFactory,
      httpClient,
      authenticator
    );

    HttpUriRequest httpRequest = createMock(HttpUriRequest.class);
    HttpResponse httpResponse = createResponse(
      HttpStatus.SC_OK,
      "content"
    );

    authenticator.addCredentials(httpRequest);
    expect(httpClient.execute(httpRequest))
      .andReturn(httpResponse);
    replayAll();

    RestfulResponse response = classUnderTest.executeRequest(httpRequest);
    assertEquals(HttpStatus.SC_OK, response.getStatusCode());
    assertEquals("content", IOUtils.toString(response.getContentStream()));
    verifyAll();
  }

  @Test
  public void executeRequest_IOException() throws Exception {
    classUnderTest = new RestfulDatabase(
      requestFactory,
      httpClient,
      authenticator
    );
    HttpUriRequest httpRequest = createMock(HttpUriRequest.class);
    
    IOException exception = new IOException();
    authenticator.addCredentials(httpRequest);
    expect(httpClient.execute(httpRequest))
      .andThrow(exception);
    expect(httpRequest.getMethod())
      .andReturn("GET");
    expect(httpRequest.getURI())
      .andReturn(java.net.URI.create("http://example.com/"));
    replayAll();

    try {
      classUnderTest.executeRequest(httpRequest);
      fail("expected DatabaseError");
    } catch (DatabaseError e) { 
      assertEquals(exception, e.getCause());
    }
    verifyAll();
  }
}
