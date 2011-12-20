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
import eu.baltrad.bdb.oh5.Source;

import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;

import java.io.*;
import java.net.URI;
import java.util.List;
import java.util.UUID;

public class RestfulDatabase implements Database {
  private HttpClient httpClient;
  private RequestFactory requestFactory;

  public RestfulDatabase(String serverUri) {
    this(URI.create(serverUri));
  }
  
  public RestfulDatabase(URI serverUri) {
    this(
      new DefaultRequestFactory(serverUri),
      new DefaultHttpClient(
        new ThreadSafeClientConnManager()
      )
    );
  }

  public RestfulDatabase(RequestFactory requestFactory,
                         HttpClient httpClient) {
    this.requestFactory = requestFactory;
    this.httpClient = httpClient;
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
   * @see eu.baltrad.bdb.db.Database#getSources()
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

  @Override
  public void close() {
    httpClient.getConnectionManager().shutdown();
  }
  
  protected RestfulResponse executeRequest(HttpUriRequest request) {
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
