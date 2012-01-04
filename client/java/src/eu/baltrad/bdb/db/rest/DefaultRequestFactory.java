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
import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.FileQuery;

import org.apache.commons.io.IOUtils;

import org.apache.http.client.methods.HttpDelete;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.client.utils.URIUtils;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.entity.StringEntity;

import java.io.InputStream;
import java.io.IOException;
import java.net.URI;
import java.util.UUID;

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
    entity.setContentType("application/x-hdf5");
    result.setEntity(entity);
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
  public HttpUriRequest createGetFileContentRequest(UUID uuid) {
    String uuidString = uuid.toString();
    return new HttpGet(getRequestUri("file/" + uuidString));
  }

  @Override
  public HttpUriRequest createQueryFileRequest(FileQuery query) {
    HttpPost result = new HttpPost(getRequestUri("query/file"));
    try {
      String queryJson = jsonUtil.jsonToString(jsonUtil.toJson(query));
      StringEntity entity = new StringEntity(queryJson);
      entity.setContentType("application/json");
      result.setEntity(entity);
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
      StringEntity entity = new StringEntity(queryJson);
      entity.setContentType("application/json");
      result.setEntity(entity);
    } catch (java.io.UnsupportedEncodingException e) {
      throw new RuntimeException(e);
    }
    return result;
  }

  @Override
  public HttpUriRequest createGetSourcesRequest() {
    return new HttpGet(getRequestUri("source/"));
  }
  
  protected URI getRequestUri(String path) {
    return URIUtils.resolve(serverUri, path);
  }
}
