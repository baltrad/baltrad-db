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

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import java.io.*;
import java.net.URL;
import java.util.*;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;

public class AttributeDefinitions {
  private Map<String, String> typeMap;
  private static AttributeDefinitions defaultInstance;

  static {
    InputStream xmlStream =
      AttributeDefinitions.class.getResourceAsStream("defaultAttributes.xml");
    defaultInstance = load(xmlStream);
  }
  
  public static AttributeDefinitions load(InputStream xmlStream) {
    AttributeDefinitions result = new AttributeDefinitions();
    Document doc = parseXml(xmlStream);
    doc.getDocumentElement().normalize();
    NodeList nodeList = doc.getElementsByTagName("attribute");

    for (int i=0; i < nodeList.getLength(); i++) {
      Node node = nodeList.item(i);
      result.addDefinitionFromXmlNode(node);
    }

    return result;
  }

  private static Document parseXml(InputStream xmlStream) {
    try {
      DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
      DocumentBuilder db = dbf.newDocumentBuilder();
      return db.parse(xmlStream);
    } catch (javax.xml.parsers.ParserConfigurationException e) {
      throw new RuntimeException(e);
    } catch (org.xml.sax.SAXException e) {
      throw new RuntimeException(e);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  public AttributeDefinitions() {
    typeMap = new HashMap<String, String>();
  }

  public static AttributeDefinitions getDefaultInstance() {
    return defaultInstance;
  }

  public String getAttributeType(String name) {
    String result = typeMap.get(name);
    if (result == null)
      throw new IllegalArgumentException("attribute `" + name + "` not found");

    return result;
  }

  protected void addDefinitionFromXmlNode(Node node) {
    NamedNodeMap nodeAttrs = node.getAttributes();
    String name = nodeAttrs.getNamedItem("name").getNodeValue();
    String type = nodeAttrs.getNamedItem("type").getNodeValue();
    typeMap.put(name, type);
  }
}
