package eu.baltrad.fc;

import eu.baltrad.fc.FileCatalogError;

public class DatabaseError extends FileCatalogError {
  static final long serialVersionUID = 0;

  public DatabaseError(String message) {
  	super(message);
  }
}
