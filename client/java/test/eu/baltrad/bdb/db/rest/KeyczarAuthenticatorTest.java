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

import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.client.methods.HttpPost;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import org.keyczar.Signer;

public class KeyczarAuthenticatorTest extends EasyMockSupport {
  private static interface KeyczarAuthenticatorMethods {
    public String createSignableString(HttpUriRequest request);
  }

  KeyczarAuthenticatorMethods methods;
  Signer signer;
  KeyczarAuthenticator classUnderTest;

  @Before
  public void setUp() {
    methods = createMock(KeyczarAuthenticatorMethods.class);
    signer = createMock(Signer.class);
    classUnderTest = new KeyczarAuthenticator(signer, "keyname");
  }

  @Test
  public void addCredentials() throws Exception {
    classUnderTest = new KeyczarAuthenticator(signer, "keyname") {
      @Override
      public String createSignableString(HttpUriRequest request) {
        return methods.createSignableString(request);
      }
    };

    HttpUriRequest request = createMock(HttpUriRequest.class);
    expect(methods.createSignableString(request))
      .andReturn("signable");
    expect(signer.sign("signable"))
      .andReturn("signature");
    request.addHeader(
      "Authorization",
      "bdb-keyczar keyname:signature"
    );
    replayAll();
    
    classUnderTest.addCredentials(request);
    verifyAll();
  }

  @Test
  public void createSignableString() {
    String expected = 
      "POST\n" +
      "/path/to/resource\n" +
      "a23ae94a35970e9689d30a56d954ef36\n" +
      "application/json\n" +
      "Tue, 10 Jan 2012 19:03:34 GMT";
    
    HttpPost request = new HttpPost("http://www.example.com/path/to/resource");
    request.addHeader("content-md5", "a23ae94a35970e9689d30a56d954ef36");
    request.addHeader("content-type", "application/json");
    request.addHeader("date", "Tue, 10 Jan 2012 19:03:34 GMT");
    assertEquals(expected, classUnderTest.createSignableString(request));
  }

  @Test
  public void createSignableString_entity() {
    String expected = 
      "POST\n" +
      "/path/to/resource\n" +
      "a23ae94a35970e9689d30a56d954ef36\n" +
      "application/json\n" +
      "Tue, 10 Jan 2012 19:03:34 GMT";

  }
}
