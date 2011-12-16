package eu.baltrad.bdb.oh5;

import java.util.Collection;

public interface Node {
  /**
   * Get the name of this node
   */
  String getName();

  /**
   * Get the parent node of this node
   * @return null if this node does not have a parent
   */
  Node getParent();

  /**
   * Set node as this nodes parent
   */
  void setParent(Node parent);
  
  /**
   * Add a child to this node.
   *
   * If a child with such a name already exists, it is replaced. 
   */
  public void addChild(Node node);

  /**
   * Get the child nodes of this node
   */
  public Collection<Node> getChildren();
    
  /**
   * Get a child node by name
   * @return null if no child is found
   */
  public Node getChild(String name);  
  
  /**
   * Get the full path of this node
   */
  public String getPath();
}
