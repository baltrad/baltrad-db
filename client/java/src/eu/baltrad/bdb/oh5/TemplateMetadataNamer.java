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

package eu.baltrad.bdb.oh5;

import java.util.*;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 * Give names to Metadata instances according to a template.
 *
 * The templates are strings where ${placeholder} denotes an attribute value
 * to be looked up from the metadata (e.g '/what/object'). If the metadata
 * does not contain the attribute, 'null' is placed in the resulting string
 * instead.
 */
public class TemplateMetadataNamer implements MetadataNamer {
  private String tmpl;

  private static final Pattern pattern;

  static {
    pattern = Pattern.compile(
      "\\$(?:" +
        "(\\$)|" + // group 1 matches escaped delimiter ($$)
        "\\{([_/a-z][_/a-z0-9]*)\\}" + // group 2 matches the placeholder
      ")",
      Pattern.CASE_INSENSITIVE
    );
  }

  public TemplateMetadataNamer(String tmpl) {
    this.tmpl = tmpl;
  }

  public void setTemplate(String tmpl) {
    this.tmpl = tmpl;
  }

  public String getTemplate() {
    return this.tmpl;
  }
  
  @Override
  public String name(Metadata metadata) {
    StringBuffer result = new StringBuffer();
    Matcher m = pattern.matcher(tmpl);
    while (m.find()) {
      String placeholder = m.group(2);
      if (placeholder != null) {
        m.appendReplacement(result, getAttributeValue(metadata, placeholder));
      }
    }
    m.appendTail(result);
    return result.toString();
  }

  private static String getAttributeValue(Metadata metadata,
                                          String attrPath) {
    Attribute attr = metadata.getAttribute(attrPath);
    if (attr != null) {
      return attr.toString();
    }
    return "null";
  }
}
