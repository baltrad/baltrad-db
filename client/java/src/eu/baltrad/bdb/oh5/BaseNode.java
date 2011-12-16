package eu.baltrad.bdb.oh5;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.Map;
import java.util.HashMap;
import java.util.Collection;

public abstract class BaseNode implements Node {
  private String name;
  private WeakReference<Node> parent;
  private Map<String, Node> children;

  public BaseNode(String name) {
    this.name = name;
    this.parent = new WeakReference<Node>(null);
    this.children = new HashMap<String, Node>();
  }

  @Override
  public String getName() {
    return name;
  }

  @Override
  public Node getParent() {
    return parent.get();
  }
  
  @Override
  public void setParent(Node node) {
    this.parent = new WeakReference<Node>(node);
  }
  
  @Override
  public void addChild(Node node) {
    node.setParent(this);
    Node old = children.put(node.getName(), node);
    if (old != null) {
      old.setParent(null);
    }
  }

  @Override
  public Collection<Node> getChildren() {
    return children.values();
  }
    
  @Override
  public Node getChild(String name) {
    return children.get(name);
  }  

  @Override
  public String getPath() {
    String parent_path = "/";
    Node parentNode = parent.get();
    if (parentNode != null) {
      parent_path = parentNode.getPath();
    }
    return (new File(parent_path, name)).toString();
  }
}
