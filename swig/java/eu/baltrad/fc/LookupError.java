package eu.baltrad.fc;

import eu.baltrad.fc.FileCatalogError;

public class LookupError extends FileCatalogError {
  static final long serialVersionUID = 0;

  public LookupError(String message) {
  	super(message);
  }
}
