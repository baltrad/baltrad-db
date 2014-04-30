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
import java.util.UUID;

import org.apache.commons.io.IOUtils;
import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.client.utils.URIUtils;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.entity.StringEntity;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

public final class DefaultRequestFactory implements RequestFactory {
  private final URI serverUri;
  private JsonUtil jsonUtil;
  
  public DefaultRequestFactory(URI serverUri) {
    try {
      this.serverUri = URIUtils.createURI(
        serverUri.getScheme(),
        serverUri.getHost(),
        serverUri.getPort(),
        serverUri.getPath() != null ? serverUri.getPath() : "/",
        "",
        ""
      );
    } catch (java.net.URISyntaxException e) {
      throw new IllegalArgumentException("invalid serverUri: " + serverUri);
    }
    this.jsonUtil = new JsonUtil();
  }

  @Override
  public HttpUriRequest createStoreFileRequest(InputStream fileContent) {
    HttpPost result = new HttpPost(getRequestUri("file/"));
    
    // XXX: this should be InputStreamEntity, but the server side
    //      has trouble with chunked encoding ATM.
    ByteArrayEntity entity = null;
    try {
      entity = new ByteArrayEntity(IOUtils.toByteArray(fileContent));
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    result.setEntity(entity);
    result.addHeader("content-type", "application/x-hdf5");
    return result;
  }

  @Override
  public HttpUriRequest createRemoveFileEntryRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpDelete(getRequestUri("file/" + uuidString));
  }

  @Override
  public HttpUriRequest createRemoveAllFileEntriesRequest() {
    return new HttpDelete(getRequestUri("file/"));
  }
  
  @Override
  public HttpUriRequest createGetFileEntryRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpGet(getRequestUri("file/" + uuidString + "/metadata"));
  }

  @Override
  public HttpUriRequest createRemoveFilesByCountRequest(int limit, int nritems) {
    return new HttpDelete(getRequestUri("file/count/"+limit+"/"+nritems));
  }
  
  @Override
  public HttpUriRequest createRemoveFilesByAgeRequest(DateTime age, int nritems) {
    String dstr = age.getDate().toIsoString() + age.getTime().toIsoString();
    return new HttpDelete(getRequestUri("file/age/"+dstr+"/"+nritems));
  }
  
  @Override
  public HttpUriRequest createGetFileCountRequest() {
    return new HttpGet(getRequestUri("file/count"));
  }
  
  @Override
  public HttpUriRequest createGetFileContentRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpGet(getRequestUri("file/" + uuidString));
  }

  @Override
  public HttpUriRequest createQueryFileRequest(FileQuery query) {
    HttpPost result = new HttpPost(getRequestUri("query/file"));
    try {
      String queryJson = jsonUtil.jsonToString(jsonUtil.toJson(query));
      StringEntity entity = new StringEntity(queryJson, "utf-8");
      result.setEntity(entity);
      result.addHeader("content-type", "application/json; charset=utf-8");
    } catch (java.io.UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
    return result;
  }

  @Override
  public HttpUriRequest createQueryAttributeRequest(AttributeQuery query) {
    HttpPost result = new HttpPost(getRequestUri("query/attribute"));
    try {
      String queryJson = jsonUtil.jsonToString(jsonUtil.toJson(query));
      StringEntity entity = new StringEntity(queryJson, "utf-8");
      result.setEntity(entity);
      result.addHeader("content-type", "application/json; charset=utf-8");
    } catch (java.io.UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
    return result;
  }

  @Override
  public HttpUriRequest createGetSourcesRequest() {
    return new HttpGet(getRequestUri("source/"));
  }
  
  @Override
  public HttpUriRequest createAddSourceRequest(Source source) {
    HttpPost result = new HttpPost(getRequestUri("source/"));
    try {
      String jsonSource = jsonUtil.jsonToString(jsonUtil.toJson(source));
      StringEntity entity = new StringEntity(jsonSource, "utf-8");
      result.setEntity(entity);
      result.addHeader("content-type", "application/json; charset=utf-8");
    } catch (java.io.UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
    return result;
  }
  
  @Override
  public HttpUriRequest createUpdateSourceRequest(Source source) {
    HttpPut result = new HttpPut(getRequestUri("source/"));
    try {
      String jsonSource = jsonUtil.jsonToString(jsonUtil.toJson(source));
      StringEntity entity = new StringEntity(jsonSource, "utf-8");
      result.setEntity(entity);
      result.addHeader("content-type", "application/json; charset=utf-8");
    } catch (java.io.UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
    return result;
  }
  
  @Override
  public HttpUriRequest createDeleteSourceRequest(String source) {
    HttpDelete result = new HttpDelete(getRequestUri("source/"+source));
    result.addHeader("content-type", "application/json; charset=utf-8");
    return result;
  }
  
  protected URI getRequestUri(String path) {
    return URIUtils.resolve(serverUri, path);
  }
}
