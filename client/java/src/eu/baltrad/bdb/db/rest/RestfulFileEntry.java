package eu.baltrad.bdb.db.rest;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.Time;

import java.io.File;
import java.io.InputStream;
import java.util.UUID;

@SuppressWarnings("deprecation")
public final class RestfulFileEntry implements FileEntry {
  private final Metadata metadata;

  public RestfulFileEntry(Metadata metadata) {
    this.metadata = metadata;
  }
  
  /**
   * @see eu.baltrad.bdb.db.FileEntry#getMetadata()
   */
  @Override
  public Metadata getMetadata() {
    return metadata;
  }

  /**
   * @see eu.baltrad.bdb.db.FileEntry#getUuid()
   */
  @Override
  public UUID getUuid() {
    String uuid = metadata.getAttribute("/_bdb/uuid").toString();
    return UUID.fromString(uuid);
  }

  /**
   * @see eu.baltrad.bdb.db.FileEntry#getStoredDate()
   */
  @Override
  public Date getStoredDate() {
    return metadata.getAttribute("/_bdb/stored_date").toDate();
  }

  /**
   * @see eu.baltrad.bdb.db.FileEntry#getStoredTime()
   */
  @Override
  public Time getStoredTime() {
    return metadata.getAttribute("/_bdb/stored_time").toTime();
  }

  /**
   * @see eu.baltrad.bdb.db.FileEntry#getSource()
   */
  @Override
  public Source getSource() {
    String sourceStr = metadata.getAttribute("/_bdb/source").toString();
    return Source.fromString(sourceStr);
  }

  /**
   * @see eu.baltrad.bdb.db.FileEntry#getContentStream()
   */
  @Override
  public int getContentSize() {
    return metadata.getAttribute("/_bdb/file_size").toLong().intValue();
  }
  
  /**
   * @see eu.baltrad.bdb.db.FileEntry#getContentStream()
   */
  @Override
  public InputStream getContentStream() {
    return null;
  }

  @Override
  public void writeToFile(File file) {
    
  }
}
