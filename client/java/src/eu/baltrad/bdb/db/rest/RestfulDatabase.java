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
import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.db.DatabaseError;
import eu.baltrad.bdb.db.DuplicateEntry;
import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.db.SourceManager;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;

import java.io.*;
import java.net.URI;
import java.util.List;
import java.util.UUID;

/**
 * 
 */
public class RestfulDatabase implements Database, SourceManager {
  private HttpClient httpClient;
  private RequestFactory requestFactory;
  private Authenticator authenticator;

  public RestfulDatabase(String serverUri) {
    this(serverUri, new NullAuthenticator());
  }

  public RestfulDatabase(String serverUri, int maxconnections) {
    this(serverUri, new NullAuthenticator(), maxconnections);
  }

  public RestfulDatabase(URI serverUri) {
    this(serverUri, new NullAuthenticator());
  }

  public RestfulDatabase(URI serverUri, int maxconnections) {
    this(serverUri, new NullAuthenticator(), maxconnections);
  }

  public RestfulDatabase(String serverUri, Authenticator authenticator) {
    this(URI.create(serverUri), authenticator);
  }

  public RestfulDatabase(String serverUri, Authenticator authenticator, int maxconnections) {
    this(URI.create(serverUri), authenticator, maxconnections);
  }

  public RestfulDatabase(URI serverUri, Authenticator authenticator) {
    this(
      new DefaultRequestFactory(serverUri),
      new DefaultHttpClient(
        new ThreadSafeClientConnManager()
      ),
      authenticator
    );
  }

  public RestfulDatabase(URI serverUri, Authenticator authenticator, int maxconnections) {
    this.requestFactory = new DefaultRequestFactory(serverUri);
    ThreadSafeClientConnManager tsccm = new ThreadSafeClientConnManager();
    tsccm.setDefaultMaxPerRoute(maxconnections);
    tsccm.setMaxTotal(maxconnections);
    this.httpClient = new DefaultHttpClient(tsccm);
    this.authenticator = authenticator;
  }

  public RestfulDatabase(RequestFactory requestFactory,
                         HttpClient httpClient,
                         Authenticator authenticator) {
    this.requestFactory = requestFactory;
    this.httpClient = httpClient;
    this.authenticator = authenticator;
  }

  /**
   * @see eu.baltrad.bdb.db.Database#store(InputStream)
   */
  @Override
  public RestfulFileEntry store(InputStream fileContent) {
    HttpUriRequest request =
      requestFactory.createStoreFileRequest(fileContent);
    RestfulResponse response = executeRequest(request);

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_CREATED) {
        return new RestfulFileEntry(this, response.getMetadata());
      } else if (statusCode == HttpStatus.SC_CONFLICT) {
        throw new DuplicateEntry("file already stored");
      } else {
        throw new DatabaseError("Failed to store file, status code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#removeFileEntry(UUID)
   */
  @Override
  public boolean removeFileEntry(UUID uuid) {
    HttpUriRequest request =
      requestFactory.createRemoveFileEntryRequest(uuid);
    RestfulResponse response = executeRequest(request);
    
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_NO_CONTENT) {
        return true;
      } else if (statusCode == HttpStatus.SC_NOT_FOUND) {
        return false;
      } else {
        throw new DatabaseError("unhandled response code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#removeAllFileEntries
   */
  @Override
  public void removeAllFileEntries() {
    HttpUriRequest request =
      requestFactory.createRemoveAllFileEntriesRequest();
    RestfulResponse response = executeRequest(request);
    
    try {
      int statusCode = response.getStatusCode();
      if (statusCode != HttpStatus.SC_NO_CONTENT) {
        throw new DatabaseError("unhandled response code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#getFileEntryByUuid(UUID)
   */
  @Override
  public RestfulFileEntry getFileEntry(UUID uuid) {
    HttpUriRequest request = requestFactory.createGetFileEntryRequest(uuid);
    RestfulResponse response = executeRequest(request);

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return new RestfulFileEntry(this, response.getMetadata());
      } else if (statusCode == HttpStatus.SC_NOT_FOUND) {
        return null;
      } else {
        throw new DatabaseError("unhandled response code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#removeFilesByCount(int, int)
   */
  @Override
  public int removeFilesByCount(int limit, int nritems) {
    HttpUriRequest request = requestFactory.createRemoveFilesByCountRequest(limit, nritems);
    RestfulResponse response = executeRequest(request);
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getJsonContent().get("numberOfFilesRemoved").getValueAsInt();
      } else {
        throw new DatabaseError("unhandled response code: " +
            Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  public int removeFilesByAge(DateTime age, int nritems) {
    HttpUriRequest request = requestFactory.createRemoveFilesByAgeRequest(age, nritems);
    RestfulResponse response = executeRequest(request);
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getJsonContent().get("numberOfFilesRemoved").getValueAsInt();
      } else {
        throw new DatabaseError("unhandled response code: " +
            Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }
  
  /**
   * @see eu.baltrad.bdb.db.Database#getFileCount()
   */
  @Override
  public long getFileCount() {
    HttpUriRequest request = requestFactory.createGetFileCountRequest();
    RestfulResponse response = executeRequest(request);
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getJsonContent().get("numberOfFiles").getValueAsLong();
      } else {
        throw new DatabaseError("unhandled response code: " +
            Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }
  
  /**
   * @see eu.baltrad.bdb.db.Database#getFileContent(UUID)
   */
  @Override
  public InputStream getFileContent(UUID uuid) {
    HttpUriRequest request = requestFactory.createGetFileContentRequest(uuid);
    RestfulResponse response = executeRequest(request);

    int statusCode = response.getStatusCode();
    if (statusCode == HttpStatus.SC_OK) {
      return response.getContentStream();
    } else if (statusCode == HttpStatus.SC_NOT_FOUND) {
      response.close();
      return null;
    } else {
      response.close();
      throw new DatabaseError("unhandled response code: " +
                              Integer.toString(statusCode));
    }
  }
  
  /**
   * @see eu.baltrad.bdb.db.Database#execute(FileQuery)
   */
  @Override
  public RestfulFileResult execute(FileQuery query) {
    HttpUriRequest request = requestFactory.createQueryFileRequest(query);
    RestfulResponse response = executeRequest(request);

    try {
      int statusCode = response.getStatusCode();
      if (statusCode != HttpStatus.SC_OK) {
        throw new DatabaseError("query failed, response code: " +
                                Integer.toString(statusCode));
      }
      return new RestfulFileResult(this, response.getResultRows());
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#execute(AttributeQuery)
   */
  @Override
  public RestfulAttributeResult execute(AttributeQuery query) {
    HttpUriRequest request =
      requestFactory.createQueryAttributeRequest(query);
    RestfulResponse response = executeRequest(request);

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return new RestfulAttributeResult(response.getResultRows());
      } else {
        throw new DatabaseError("query failed, response code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.Database#close()
   */
  @Override
  public void close() {
    httpClient.getConnectionManager().shutdown();
  }

  /**
   * @see eu.baltrad.bdb.db.Database#getSourceManager()
   */
  public SourceManager getSourceManager() {
    return this;
  }
  
  /**
   * @see eu.baltrad.bdb.db.SourceManager#getSources()
   */
  @Override  
  public List<Source> getSources() {
    HttpUriRequest request = requestFactory.createGetSourcesRequest();
    RestfulResponse response = executeRequest(request);
    
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getSources();
      } else {
        throw new DatabaseError("source listing failed, response code: " +
                                Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.SourceManager#add(Source)
   */
  @Override
  public void add(Source src) {
    HttpUriRequest request = requestFactory.createAddSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_CREATED) {
        // no op
      } else if (statusCode == HttpStatus.SC_CONFLICT) {
        throw new DuplicateEntry("Source already exists");
      } else {
        throw new DatabaseError("Could not add source, response code: " + Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.SourceManager#update(Source)
   */
  @Override
  public void update(Source src) {
    HttpUriRequest request = requestFactory.createUpdateSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    try {
      int statusCode = response.getStatusCode();
      if (statusCode != HttpStatus.SC_NO_CONTENT) {
        throw new DatabaseError("Could not update source, response code: " + Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  /**
   * @see eu.baltrad.bdb.db.SourceManager#remove(Source)
   */
  @Override
  public boolean remove(String src) {
    HttpUriRequest request = requestFactory.createDeleteSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    boolean result = false;
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_NO_CONTENT) {
        result = true;
      } else if (statusCode == HttpStatus.SC_NOT_FOUND) {
        result = false;
      } else {
        throw new DatabaseError("Could not remove source, response code: " + Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
    return result;
  }

  protected RestfulResponse executeRequest(HttpUriRequest request) {
    authenticator.addCredentials(request);
    try {
      return new RestfulResponse(httpClient.execute(request));
    } catch (IOException e) {
      throw new DatabaseError(
        "HTTP " + request.getMethod() + " to " + request.getURI() + " failed",
        e
      );
    }
  }
}
