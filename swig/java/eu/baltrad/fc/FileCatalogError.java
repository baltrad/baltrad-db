package eu.baltrad.fc;

public class FileCatalogError extends RuntimeException {
  static final long serialVersionUID = 0;

  public FileCatalogError(String message) {
    super(message);
  }
}
