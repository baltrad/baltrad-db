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

import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.List;
import java.util.UUID;

import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.db.DatabaseError;
import eu.baltrad.bdb.db.DuplicateEntry;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.db.SourceManager;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

/**
 * 
 */
public class RestfulDatabase implements Database, SourceManager {
  private HttpClient httpClient;
  private RequestFactory requestFactory;
  private Authenticator authenticator;
  private RestfulFileEntryCache fileEntryCache;

  private static final int defaultFileEntryCacheSize = 0;
  private static Logger logger = LogManager.getLogger(RestfulDatabase.class);
  
  public RestfulDatabase(String serverUri, int maxconnections) {
    this(serverUri, new NullAuthenticator(), maxconnections);
  }

  public RestfulDatabase(URI serverUri, int maxconnections) {
    this(serverUri, new NullAuthenticator(), maxconnections, defaultFileEntryCacheSize);
  }

  public RestfulDatabase(String serverUri, Authenticator authenticator, int maxconnections) {
    this(URI.create(serverUri), authenticator, maxconnections, defaultFileEntryCacheSize);
  }
  
  public RestfulDatabase(String serverUri, Authenticator authenticator, int maxconnections, int fileEntryCacheSize) {
    this(URI.create(serverUri), authenticator, maxconnections, fileEntryCacheSize);
  }

  public RestfulDatabase(URI serverUri, Authenticator authenticator, int maxconnections, int fileEntryCacheSize) {
    this.requestFactory = new DefaultRequestFactory(serverUri);
    ThreadSafeClientConnManager tsccm = new ThreadSafeClientConnManager();
    tsccm.setDefaultMaxPerRoute(maxconnections);
    tsccm.setMaxTotal(maxconnections);
    this.httpClient = new DefaultHttpClient(tsccm);
    this.authenticator = authenticator;
    if (fileEntryCacheSize > 0) {
      this.fileEntryCache = new RestfulFileEntryCache(fileEntryCacheSize);      
    } else {
      this.fileEntryCache = null;
    }
  }
  
  public RestfulDatabase(String serverUri) {
    this(serverUri, new NullAuthenticator());
  }
  
  public RestfulDatabase(URI serverUri) {
    this(serverUri, new NullAuthenticator());
  }
  
  public RestfulDatabase(String serverUri, Authenticator authenticator) {
    this(URI.create(serverUri), authenticator);
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

  public RestfulDatabase(RequestFactory requestFactory,
                         HttpClient httpClient,
                         Authenticator authenticator) {
    this.requestFactory = requestFactory;
    this.httpClient = httpClient;
    this.authenticator = authenticator;
    this.fileEntryCache = null;
  }

  public RestfulFileEntryCache getFileEntryCache() {
    return fileEntryCache;
  }

  public void setFileEntryCache(RestfulFileEntryCache fileEntryCache) {
    this.fileEntryCache = fileEntryCache;
  }

  /**
   * @see eu.baltrad.bdb.db.Database#store(InputStream)
   */
  @Override
  public RestfulFileEntry store(InputStream fileContent) {
    long st = System.currentTimeMillis();
    long queryTime = 0;

    HttpUriRequest request =
      requestFactory.createStoreFileRequest(fileContent);
    RestfulResponse response = executeRequest(request);
    
    queryTime = System.currentTimeMillis();

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_CREATED) {
        RestfulFileEntry newFileEntry = new RestfulFileEntry(this, response.getMetadata());
        if (fileEntryCache != null) {
          fileEntryCache.addFileEntry(newFileEntry);          
        }
        logger.info("bdb.RestfulDatabase.store: Took " + (queryTime - st) + " ms");
        return newFileEntry;
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

  @Override
  public Metadata queryFileMetadata(InputStream fileContent) {
    long st = System.currentTimeMillis();
    
    HttpUriRequest request =
        requestFactory.createQueryFileMetadata(fileContent);
    RestfulResponse response = executeRequest(request);

    logger.debug("bdb.RestfulDatabase.queryFileMetadata: Took " + (System.currentTimeMillis() - st) + " ms");

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getMetadata();
      } else {
        throw new DatabaseError("Failed to query file for metadata store file, status code: " +
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
    long st = System.currentTimeMillis();
    HttpUriRequest request =
      requestFactory.createRemoveFileEntryRequest(uuid);
    RestfulResponse response = executeRequest(request);
    
    logger.debug("bdb.RestfulDatabase.removeFileEntry: Took " + (System.currentTimeMillis() - st) + " ms");

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_NO_CONTENT) {
        if (fileEntryCache != null) {
          fileEntryCache.removeFileEntry(uuid);          
        }
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
    long st = System.currentTimeMillis();
    HttpUriRequest request =
      requestFactory.createRemoveAllFileEntriesRequest();
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.removeAllFileEntries: Took " + (System.currentTimeMillis() - st) + " ms");
    
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
    long st = System.currentTimeMillis();
    if (fileEntryCache != null) {
      RestfulFileEntry cachedFileEntry = fileEntryCache.getFileEntry(uuid);
      
      if (cachedFileEntry != null) {
        logger.debug("bdb.RestfulDatabase.getFileEntry: Returning cache entry after " + (System.currentTimeMillis() - st) + " ms");
        return cachedFileEntry;
      }     
    }
    
    HttpUriRequest request = requestFactory.createGetFileEntryRequest(uuid);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getFileEntry: Executed request in " + (System.currentTimeMillis() - st) + " ms");

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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createRemoveFilesByCountRequest(limit, nritems);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.removeFilesByCount: Executed request in " + (System.currentTimeMillis() - st) + " ms");

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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createRemoveFilesByAgeRequest(age, nritems);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.removeFilesByAge: Executed request in " + (System.currentTimeMillis() - st) + " ms");
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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetFileCountRequest();
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getFileCount: Executed request in " + (System.currentTimeMillis() - st) + " ms");
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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetFileContentRequest(uuid);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getFileContent: Executed request in " + (System.currentTimeMillis() - st) + " ms");

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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createQueryFileRequest(query);
    RestfulResponse response = executeRequest(request);
    logger.info("bdb.RestfulDatabase.execute(FileQuery): Executed request in " + (System.currentTimeMillis() - st) + " ms");

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
    long st = System.currentTimeMillis();
    HttpUriRequest request =
      requestFactory.createQueryAttributeRequest(query);
    RestfulResponse response = executeRequest(request);
    logger.info("bdb.RestfulDatabase.execute(AttributeQuery): Executed request in " + (System.currentTimeMillis() - st) + " ms");

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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetSourcesRequest();
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getSources(): Executed request in " + (System.currentTimeMillis() - st) + " ms");
    
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
  public Source getSource(String name) {
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetSourceRequest(name);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getSource(String): Executed request in " + (System.currentTimeMillis() - st) + " ms");
    
    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getSource();
      } else {
        throw new DatabaseError("source retrival, response code: " +
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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createAddSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.add(Source): Executed request in " + (System.currentTimeMillis() - st) + " ms");
    
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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createUpdateSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.update(Source): Executed request in " + (System.currentTimeMillis() - st) + " ms");
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
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createDeleteSourceRequest(src);
    RestfulResponse response = executeRequest(request);
    boolean result = false;
    logger.debug("bdb.RestfulDatabase.remove(String): Executed request in " + (System.currentTimeMillis() - st) + " ms");
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

  @Override
  public List<Source> getParentSources() {
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetParentSourcesRequest();
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getParentSources(): Executed request in " + (System.currentTimeMillis() - st) + " ms");

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getSources();
      } else {
        throw new DatabaseError("source listing failed, response code: " + Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
  }

  @Override
  public List<Source> getSourcesWithParent(String parent) {
    long st = System.currentTimeMillis();
    HttpUriRequest request = requestFactory.createGetSourcesWithParent(parent);
    RestfulResponse response = executeRequest(request);
    logger.debug("bdb.RestfulDatabase.getSourcesWithParent(): Executed request in " + (System.currentTimeMillis() - st) + " ms");

    try {
      int statusCode = response.getStatusCode();
      if (statusCode == HttpStatus.SC_OK) {
        return response.getSources();
      } else {
        throw new DatabaseError("source listing failed, response code: " + Integer.toString(statusCode));
      }
    } finally {
      response.close();
    }
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
