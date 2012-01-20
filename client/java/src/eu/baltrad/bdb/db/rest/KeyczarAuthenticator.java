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

import org.apache.commons.codec.digest.DigestUtils;
import org.apache.commons.lang3.StringUtils;

import org.apache.http.Header;
import org.apache.http.client.methods.HttpUriRequest;

import org.keyczar.Signer;
import org.keyczar.exceptions.KeyczarException;

import java.io.*;
import java.util.*;

/**
 * authenticate HTTP requests by signing with Keyczar
 */
public class KeyczarAuthenticator implements Authenticator {
  Signer signer;
  String keyName;

  private final String[] INTERESTING_HEADERS = new String[]{
    "content-md5", "content-type", "date"
  };

  /**
   * Construct using an existing signer.
   * 
   * @param signer the Signer to user
   * @param keyName the name to use in the credentials
   */
  public KeyczarAuthenticator(Signer signer, String keyName) {
    this.signer = signer;
    this.keyName = keyName;
  }

  /**
   * Construct using a path to the key.
   *
   * @param keyPath path to the key
   *
   * keyName is taken to be the basename of the keyPath
   */
  public KeyczarAuthenticator(File keyPath) {
    this(keyPath, keyPath.getName());
  }
  
  /**
   * Construct using a path to the key with explicit keyName.
   *
   * @param keyPath path to the key
   * @param keyName the name to use in the credentials
   */
  public KeyczarAuthenticator(File keyPath, String keyName) {
    this(createSigner(keyPath), keyName);
  }
  
  @Override
  public void addCredentials(HttpUriRequest request) {
    String signable = createSignableString(request);
    String signature;
    try {
      signature = signer.sign(signable);
    } catch (KeyczarException e) {
      throw new RuntimeException("couldn't sign request", e);
    }
    request.addHeader(
      "Authorization",
      "bdb-keyczar " + keyName + ":" + signature
    );
  }

  protected String createSignableString(HttpUriRequest request) {
    List<String> result = new ArrayList<String>();
    result.add(request.getMethod());
    result.add(request.getURI().getPath());
    for (String headerName : INTERESTING_HEADERS) {
      Header header = request.getFirstHeader(headerName);
      if (header != null) {
        String headerValue = header.getValue();
        headerValue = StringUtils.strip(headerValue);
        result.add(headerValue);
      }
    }
    return StringUtils.join(result, '\n');
  }

  static protected Signer createSigner(File keyPath) {
    try {
      return new Signer(keyPath.toString());
    } catch (KeyczarException e) {
      throw new RuntimeException("could't create Signer", e);
    }
  }
}
