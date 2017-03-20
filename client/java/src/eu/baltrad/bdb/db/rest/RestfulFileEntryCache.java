/*
Copyright 2017 Swedish Meteorological and Hydrological Institute, SMHI,

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
package eu.baltrad.bdb.db.rest;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.UUID;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

/**
 * A cache implementation that keeps RestfulFileEntry objects cached, with UUIDs as keys. 
 * It is used as a cache between the baltrad-db application and the database, to 
 * avoid high frequency queries towards the database for the same UUIDs.
 * 
 * The size of the cache is set in the constructor of the class. When the cache is full,
 * the oldest entry will be removed automatically when a new entry is added. The entry 
 * that is considered as the oldest is the one that was least recently accessed.
 * 
 * @author Mats Vernersson
 * @date 2017-01-26
 */
public class RestfulFileEntryCache {
  
  RestfulFileEntryHashMap fileEntries;
  CacheHitRateStatistics hitRateStats;
  
  private static Logger logger = LogManager.getLogger(RestfulFileEntryCache.class);
  
  private class RestfulFileEntryHashMap extends LinkedHashMap<UUID, RestfulFileEntry> {

    private int cacheSize;
    
    private static final long serialVersionUID = -303413871724386242L;
    
    /**
     * Constructor for the RestfulFileEntryHashMap.
     * 
     * @param cacheSize the maximum size of the cache. When this no of file entries has 
     *                  been added to the cache, the oldest entry will be removed for every
     *                  new entry that is added.
     * 
     */
    public RestfulFileEntryHashMap(int cacheSize) {
      // By setting initial capacity to (cacheSize + 1) and loadFactor to 1.0, 
      // the size of the hashMap will have a static size equal to cache size.
      // Making the hash map access-ordered by setting last argument in constructor to 'true'.
      super(cacheSize + 1, (float) 1.0, true);
      
      this.cacheSize = cacheSize;
    }

    @Override
    protected boolean removeEldestEntry(Map.Entry<UUID, RestfulFileEntry> eldest)
    {
       return this.size() > cacheSize;   
    }
  }
  
  private class CacheHitRateStatistics {
    private int resetInterval = 100;

    private int noOfHits;
    private int noOfMiss;
    
    private double currentHitRate = 0.0;
    
    public CacheHitRateStatistics() {
      resetHitStatistics();
    }
    
    public int getResetInterval() {
      return resetInterval;
    }
    
    private void resetHitStatistics() {
      noOfHits = 0;
      noOfMiss = 0;
    }
    
    public boolean lookupExecuted(boolean cacheHit) {
      boolean statReset = false;
      if (cacheHit) {
        noOfHits++;
      } else {
        noOfMiss++;
      }
      
      updateHitRate();
      
      if (noOfHits + noOfMiss >= resetInterval) {
        statReset = true;
        resetHitStatistics();
      }
      
      return statReset;
    }
    
    private void updateHitRate() {
      int noOfLookups = noOfHits + noOfMiss;
      currentHitRate = (double)noOfHits / (double)noOfLookups;
    }
    
    public double getHitRate() {
      return currentHitRate;
    }
    
  }
  
  /**
   * Constructor for the RestfulFileEntryCache.
   * 
   * @param cacheSize the maximum size of the cache. When this no of file entries has 
   *                  been added to the cache, the oldest entry will be removed for every
   *                  new entry that is added.
   */
  public RestfulFileEntryCache(int cacheSize) {
    this.fileEntries = new RestfulFileEntryHashMap(cacheSize);
    this.hitRateStats = new CacheHitRateStatistics();
  }
  
  /**
   * Adds a file entry to the cache. If the cache is full when adding an entry, the 
   * oldest entry in the cache will be removed.
   * 
   * @param entry The RestfulFileEntry to add to the cache.
   */
  public synchronized void addFileEntry(RestfulFileEntry entry) {
    fileEntries.put(entry.getUuid(), entry);      
  }
  
  /**
   * Gets a file entry with a specific UUID from the cache. Returns null if no match 
   * is found in the cache.
   * 
   * @param uuid The UUID to get a RestfulFileEntry for.
   * @return fileEntry The RestfulFileEntry in the cache with the specified UUID. null if 
   *                   no match is found
   */
  public synchronized RestfulFileEntry getFileEntry(UUID uuid) {
    RestfulFileEntry fileEntry = fileEntries.get(uuid);
    
    boolean resetLimitReached = hitRateStats.lookupExecuted(fileEntry != null);
    
    if (resetLimitReached) {
      logger.debug("File entry cache hit rate for the last " + hitRateStats.getResetInterval() + " cache lookups: " + hitRateStats.getHitRate() + 
                   ". No of files in cache: " + getNoOfEntriesInCache());      
    }
    
    return fileEntry;
  }
  
  /**
   * Removes a file entry with a specific UUID from the cache.
   * 
   * @param uuid The UUID to remove a RestfulFileEntry for.
   */
  public synchronized RestfulFileEntry removeFileEntry(UUID uuid) {
    return fileEntries.remove(uuid);
  }
  
  /**
   * Returns the number of file entries currently kept in the cache.
   * 
   * @return noOfEntriesInCache
   */
  public synchronized int getNoOfEntriesInCache() {
    return fileEntries.size();
  }
  
  /**
   * Returns the cache hit rate, i.e., the number of cache hits divided by the total 
   * number of lookups (calls to getFileEntry). The hit rate statistics is reset with a 
   * specified interval, currently to set to every 100th lookup.
   * 
   * @return noOfEntriesInCache
   */
  public double getCurrentCacheHitRate() {
    return hitRateStats.getHitRate();
  }

}
