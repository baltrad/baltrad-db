/*
Copyright 2012 Swedish Meteorological and Hydrological Institute, SMHI,

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
package eu.baltrad.bdb.db;

import java.util.List;

import eu.baltrad.bdb.oh5.Source;

/**
 * This is the manager for working with sources
 * @author Anders Henja
 * @date 2012-04-01
 */
public interface SourceManager {
  /**
   * @return a list of sources
   */
  List<Source> getSources();

  /**
   * @param src the source to add
   */
  void add(Source src);
  
  /**
   * @param src the source to update
   */
  void update(Source src);
  
  /**
   * @param src the source to remove
   * @return true if file was removed, false if file did not exist
   */
  boolean remove(String src);
  
  /**
   * @return a list of sources that are parents
   */
  List<Source> getParentSources();
  
  /**
   * Return a list of sources with specified parent 
   * @param parent the parent to look for
   * @return a list
   */
  List<Source> getSourcesWithParent(String parent);
}
