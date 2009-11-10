package eu.baltrad.fc;

import eu.baltrad.fc.FileCatalogError;

public class DuplicateEntry extends FileCatalogError {
  static final long serialVersionUID = 0;

  public DuplicateEntry(String message) {
  	super(message);
  }
}
