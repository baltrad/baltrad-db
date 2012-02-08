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

import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.Time;

import java.util.*;

public class Metadata implements Iterable<Node> {
  private Node root;

  public Metadata() {
    root = new Group("");
  }

  public Metadata(Node root) {
    this.root = root;
  }

  public Attribute getAttribute(String path) {
    try {
      return (Attribute)getNode(path);
    } catch (ClassCastException e) {
      return null;
    }
  }
  
  /**
   * add a node to this metadata
   * @param path the path of the parent node
   * @param node the node to add
   */
  public void addNode(String path, Node node) {
    Node parent = getNode(path);
    parent.addChild(node);
  }
  
  /**
   * get node by path
   * @param path the path to look the node for
   * @return node if found or null
   */
  public Node getNode(String path) {
    List<String> names = new LinkedList<String>(
      Arrays.asList(path.split("/"))
    );
    if (names.size() > 0 && "".equals(names.get(0))) {
      names.remove(0);
    }
    Node result = root;
    for (String name: names) {
      result = result.getChild(name);
      if (result == null)
        break;
    }
    return result;
  }
  
  @Override
  public Iterator<Node> iterator() {
    return new NodeIterator(root);
  }

  /**
   * parse a Source instance from /what/source
   *
   * @return the parsed source or null if /what/source is not in the metadata
   */
  public Source getSource() {
    String sourceStr = getWhatSource();
    if (sourceStr != null)
      return Source.fromString(sourceStr);
    return null;
  }

  /**
   * parse a Source instance from /_bdb/source
   *
   * @return the parsed source or null if /what/source is not in the metadata
   */
  public Source getBdbSource() {
    Attribute srcAttr = getAttribute("/_bdb/source");
    if (srcAttr == null)
      return null;
    return Source.fromString(srcAttr.toString());
  }
  
  /**
   * access the string stored in /what/source
   *
   * @return the string or null if the attribute doesn't exist
   */
  public String getWhatSource() {
    Attribute attr = getAttribute("/what/source");
    if (attr == null)
      return null;
    return attr.toString();
  }
  
  /**
   * access the string stored in /what/object
   */
  public String getWhatObject() {
    return getAttribute("/what/object").toString();
  }

  public Date getWhatDate() {
    return getAttribute("/what/date").toDate();
  }

  public Time getWhatTime() {
    return getAttribute("/what/time").toTime();
  }
}
