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
import java.net.URISyntaxException;
import java.util.UUID;

import org.apache.commons.io.IOUtils;
import org.apache.hc.client5.http.classic.methods.HttpDelete;
import org.apache.hc.client5.http.classic.methods.HttpGet;
import org.apache.hc.client5.http.classic.methods.HttpPost;
import org.apache.hc.client5.http.classic.methods.HttpPut;
import org.apache.hc.client5.http.classic.methods.HttpUriRequestBase;
import org.apache.hc.core5.http.io.entity.ByteArrayEntity;
import org.apache.hc.core5.http.io.entity.StringEntity;
import org.apache.hc.core5.http.ContentType;
import org.apache.hc.core5.net.URIBuilder;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

public final class DefaultRequestFactory implements RequestFactory {
  private final URI serverUri;
  private JsonUtil jsonUtil;
  
  public DefaultRequestFactory(URI serverUri) {
    try {
      this.serverUri = new URIBuilder(serverUri)
        .setPath(serverUri.getPath() != null ? serverUri.getPath() : "/")
        .clearParameters()
        .setFragment(null)
        .build();
    } catch (URISyntaxException e) {
      throw new IllegalArgumentException("invalid serverUri: " + serverUri);
    }
    this.jsonUtil = new JsonUtil();
  }

  @Override
  public HttpUriRequestBase createStoreFileRequest(InputStream fileContent) {
    HttpPost result = new HttpPost(getRequestUri("file/"));
    result.addHeader("content-type", "application/x-hdf5");
    
    // XXX: this should be InputStreamEntity, but the server side
    //      has trouble with chunked encoding ATM.
    ByteArrayEntity entity = null;
    try {
      entity = new ByteArrayEntity(IOUtils.toByteArray(fileContent), ContentType.create("application/x-hdf5"));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    result.setEntity(entity);
    return result;
  }

  @Override
  public HttpUriRequestBase createQueryFileMetadata(InputStream fileContent) {
    HttpPost result = new HttpPost(getRequestUri("file/metadata"));
    // XXX: this should be InputStreamEntity, but the server side
    //      has trouble with chunked encoding ATM.
    ByteArrayEntity entity = null;
    try {
      entity = new ByteArrayEntity(IOUtils.toByteArray(fileContent), ContentType.create("application/x-hdf5"));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    result.setEntity(entity);
    return result;
  }

  
  @Override
  public HttpUriRequestBase createRemoveFileEntryRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpDelete(getRequestUri("file/" + uuidString));
  }

  @Override
  public HttpUriRequestBase createRemoveAllFileEntriesRequest() {
    return new HttpDelete(getRequestUri("file/"));
  }
  
  @Override
  public HttpUriRequestBase createGetFileEntryRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpGet(getRequestUri("file/" + uuidString + "/metadata"));
  }

  @Override
  public HttpUriRequestBase createRemoveFilesByCountRequest(int limit, int nritems) {
    return new HttpDelete(getRequestUri("file/count/"+limit+"/"+nritems));
  }
  
  @Override
  public HttpUriRequestBase createRemoveFilesByAgeRequest(DateTime age, int nritems) {
    String dstr = age.getDate().toIsoString() + age.getTime().toIsoString();
    return new HttpDelete(getRequestUri("file/age/"+dstr+"/"+nritems));
  }
  
  @Override
  public HttpUriRequestBase createGetFileCountRequest() {
    return new HttpGet(getRequestUri("file/count"));
  }
  
  @Override
  public HttpUriRequestBase createGetFileContentRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpGet(getRequestUri("file/" + uuidString));
  }

  @Override
  public HttpUriRequestBase createQueryFileRequest(FileQuery query) {
    HttpPost result = new HttpPost(getRequestUri("query/file"));
    result.addHeader("content-type", "application/json; charset=utf-8");
    try {
      String queryJson = jsonUtil.jsonToString(jsonUtil.toJson(query));
      StringEntity entity = new StringEntity(queryJson, ContentType.APPLICATION_JSON);
      result.setEntity(entity);
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
    return result;
  }

  @Override
  public HttpUriRequestBase createQueryAttributeRequest(AttributeQuery query) {
    HttpPost result = new HttpPost(getRequestUri("query/attribute"));
    result.addHeader("content-type", "application/json; charset=utf-8");
    try {
      String queryJson = jsonUtil.jsonToString(jsonUtil.toJson(query));
      StringEntity entity = new StringEntity(queryJson, ContentType.APPLICATION_JSON);
      result.setEntity(entity);
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
    return result;
  }

  @Override
  public HttpUriRequestBase createGetSourcesRequest() {
    return new HttpGet(getRequestUri("source/"));
  }

  @Override
  public HttpUriRequestBase createGetSourceRequest(String name) {
    return new HttpGet(getRequestUri("source/by_name/"+name));
  }
  
  @Override
  public HttpUriRequestBase createAddSourceRequest(Source source) {
    HttpPost result = new HttpPost(getRequestUri("source/"));
    result.addHeader("content-type", "application/json; charset=utf-8");
    try {
      String jsonSource = jsonUtil.jsonToString(jsonUtil.toJson(source));
      StringEntity entity = new StringEntity(jsonSource, ContentType.APPLICATION_JSON);
      result.setEntity(entity);
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
    return result;
  }
  
  @Override
  public HttpUriRequestBase createUpdateSourceRequest(Source source) {
    HttpPut result = new HttpPut(getRequestUri("source/"));
    result.addHeader("content-type", "application/json; charset=utf-8");
    try {
      String jsonSource = jsonUtil.jsonToString(jsonUtil.toJson(source));
      StringEntity entity = new StringEntity(jsonSource, ContentType.APPLICATION_JSON);
      result.setEntity(entity);
    } catch (Exception e) {
      throw new RuntimeException(e);
    }
    return result;
  }
  
  @Override
  public HttpUriRequestBase createDeleteSourceRequest(String source) {
    HttpDelete result = new HttpDelete(getRequestUri("source/"+source));
    result.addHeader("content-type", "application/json; charset=utf-8");
    return result;
  }
  
  @Override
  public HttpUriRequestBase createGetParentSourcesRequest() {
	return new HttpGet(getRequestUri("source/parents"));
  }

  @Override
  public HttpUriRequestBase createGetSourcesWithParent(String parent) {
	return new HttpGet(getRequestUri("source/with_parent/"+parent));
  }
  
  protected URI getRequestUri(String path) {
    try {
      // Normalize: ensure base path doesn't end with / and path doesn't start with /
      String basePath = serverUri.getPath();
      if (basePath == null || basePath.isEmpty()) {
        basePath = "";
      }
      // Remove trailing slash from base
      if (basePath.endsWith("/")) {
        basePath = basePath.substring(0, basePath.length() - 1);
      }
      // Ensure path starts with /
      if (!path.startsWith("/")) {
        path = "/" + path;
      }
      
      return new URIBuilder(serverUri)
        .setPath(basePath + path)
        .build();
    } catch (URISyntaxException e) {
      throw new RuntimeException("Failed to build URI", e);
    }
  }
  
}
