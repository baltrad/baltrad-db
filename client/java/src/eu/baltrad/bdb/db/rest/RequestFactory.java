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
import eu.baltrad.bdb.util.DateTime;

import org.apache.http.client.methods.HttpUriRequest;

import java.io.InputStream;
import java.util.UUID;

public interface RequestFactory {
  HttpUriRequest createStoreFileRequest(InputStream fileContent);

  HttpUriRequest createRemoveFileEntryRequest(UUID uuid);

  HttpUriRequest createRemoveAllFileEntriesRequest();

  HttpUriRequest createGetFileEntryRequest(UUID uuid);

  HttpUriRequest createRemoveFilesByCountRequest(int limit, int nritems);
  
  HttpUriRequest createRemoveFilesByAgeRequest(DateTime age, int nritems);
  
  HttpUriRequest createGetFileCountRequest();
  
  HttpUriRequest createGetFileContentRequest(UUID uuid);

  HttpUriRequest createQueryFileRequest(FileQuery query);

  HttpUriRequest createQueryAttributeRequest(AttributeQuery query);

  HttpUriRequest createGetSourcesRequest();

  HttpUriRequest createGetSourceRequest(String name);
  
  HttpUriRequest createAddSourceRequest(Source source);
  
  HttpUriRequest createUpdateSourceRequest(Source source);
  
  HttpUriRequest createDeleteSourceRequest(String source);
  
  HttpUriRequest createGetParentSourcesRequest();

  HttpUriRequest createGetSourcesWithParent(String parent);
}
