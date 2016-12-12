package eu.baltrad.bdb.db.rest;

import java.util.UUID;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.FileResultEntry;

public class RestfulFileResultEntry implements FileResultEntry {
  
  private UUID uuid;
  private String sourceName;
  private RestfulDatabase database;
  
  public RestfulFileResultEntry(UUID uuid, String sourceName, RestfulDatabase database) {
    this.uuid = uuid;
    this.sourceName = sourceName;
    this.database = database;
  }

  @Override
  public UUID getUuid() {
    return uuid;
  }

  @Override
  public String getSourceName() {
    return sourceName;
  }

  @Override
  public FileEntry getFullFileEntry() {
    return database.getFileEntry(uuid);
  }

}
