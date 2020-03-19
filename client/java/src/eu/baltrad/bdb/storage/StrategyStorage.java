/*
Copyright 2020-2011 Swedish Meteorological and Hydrological Institute

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
package eu.baltrad.bdb.storage;

import java.io.File;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import eu.baltrad.bdb.db.FileEntry;

/**
 * Strategy provider when there are different configurable ways to store the files.
 */
public class StrategyStorage implements LocalStorage {
	private Map<String, LocalStorage> strategies = new HashMap<String, LocalStorage>();
	private String strategyId = null;
	private LocalStorage storage = null;
  private static Logger logger = LogManager.getLogger(LinkedFileCache.class);

  /**
   * Default constructor
   */
  public StrategyStorage() {
  }
  
  /**
   * Constructor
   * @param strategies the strategies
   */
	public StrategyStorage(Map<String, LocalStorage> strategies) {
		this.strategies = strategies;
	}

	/**
	 * Constructor
	 * @param strategies the various strategies
	 * @param id which of the strategies to initialize with
	 */
	public StrategyStorage(Map<String, LocalStorage> strategies, String id) {
    this.strategies = strategies;
    this.strategyId = id;
  }
	
	/**
	 * @param strategies the available strategies
	 */
	public void setStrategies(Map<String, LocalStorage> strategies) {
	  this.strategies = strategies;
	}
	
	/**
	 * @return the available strategies
	 */
	public Map<String, LocalStorage> getStrategies() {
	  return this.strategies;
	}
	
	/**
	 * @param id the id of the strategy to use
	 */
	public void setStrategy(String id) {
    this.strategyId = id;
	}

	/**
	 * @return the id of the strategy to use
	 */
  public String getStrategy() {
    return strategyId;
  }

  /**
   * Initializes the strategy selection so that correct strategy is used.
   * @throws RuntimeException if the strategyId doesn't exist as one of the available strategies.
   */
  public void init() {
    if (strategyId != null) {
      if (strategies.containsKey(strategyId)) {
        logger.info("Initializing StrategyStorage to use: " + strategyId);
        storage = strategies.get(strategyId);
      } else {
        logger.error("No storage strategy: " + strategyId);
        throw new RuntimeException("Could not initialize strategy storage: " + strategyId);
      }
    } else {
      throw new RuntimeException("Could not initialize strategy storage with null id");
    }
  }
  
  /**
   * @see LocalStorage#store(FileEntry, InputStream)
   */
	@Override
	public File store(FileEntry entry, InputStream fileContent) {
	  return storage.store(entry, fileContent);
	}

	/**
	 * @see LocalStorage#store(FileEntry)
	 */
	@Override
	public File store(FileEntry entry) {
	  return storage.store(entry);
	}

	/**
	 * @see LocalStorage#remove(FileEntry) 
	 */
	@Override
	public void remove(FileEntry entry) {
	  storage.remove(entry);
	}
}
