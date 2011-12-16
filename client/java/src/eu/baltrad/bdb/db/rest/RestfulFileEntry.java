package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;

import java.io.File;
import java.io.InputStream;
import java.util.UUID;

@SuppressWarnings("deprecation")
public final class RestfulFileEntry implements FileEntry {
  private final Metadata metadata;

  public RestfulFileEntry(Metadata metadata) {
    this.metadata = metadata;
  }
  
  @Override
  /**
   * @see eu.baltrad.bdb.db.FileEntry#getMetadata()
   */
  public Metadata getMetadata() {
    return metadata;
  }

  @Override
  /**
   * @see eu.baltrad.bdb.db.FileEntry#getUuid()
   */
  public UUID getUuid() {
    String uuid = metadata.getAttribute("/_bdb/uuid").toString();
    return UUID.fromString(uuid);
  }

  @Override
  public Source getSource() {
    return metadata.getSource();
  }
  
  @Override
  public InputStream getContentStream() {
    return null;
  }

  @Override
  public void writeToFile(File file) {
    
  }
}
