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

import java.io.InputStream;
import java.util.UUID;

import org.apache.hc.client5.http.classic.methods.HttpUriRequestBase;

import eu.baltrad.bdb.db.AttributeQuery;
import eu.baltrad.bdb.db.FileQuery;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

public interface RequestFactory {
  HttpUriRequestBase createStoreFileRequest(InputStream fileContent);
  
  HttpUriRequestBase createQueryFileMetadata(InputStream fileContent);  

  HttpUriRequestBase createRemoveFileEntryRequest(UUID uuid);
  
  HttpUriRequestBase createRemoveAllFileEntriesRequest();
  
  HttpUriRequestBase createGetFileEntryRequest(UUID uuid);
  
  HttpUriRequestBase createRemoveFilesByCountRequest(int limit, int nritems);
  
  HttpUriRequestBase createRemoveFilesByAgeRequest(DateTime age, int nritems);
  
  HttpUriRequestBase createGetFileCountRequest();
  
  HttpUriRequestBase createGetFileContentRequest(UUID uuid);
  
  HttpUriRequestBase createQueryFileRequest(FileQuery query);
  
  HttpUriRequestBase createQueryAttributeRequest(AttributeQuery query);
  
  HttpUriRequestBase createGetSourcesRequest();
  
  HttpUriRequestBase createGetSourceRequest(String name);
  
  HttpUriRequestBase createAddSourceRequest(Source source);
  
  HttpUriRequestBase createUpdateSourceRequest(Source source);
  
  HttpUriRequestBase createDeleteSourceRequest(String source);
  
  HttpUriRequestBase createGetParentSourcesRequest();
  
  HttpUriRequestBase createGetSourcesWithParent(String parent);
}
