/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_OH5_SOURCE_H
#define BRFC_OH5_SOURCE_H

#include <vector>
#include <map>
#include <string>

namespace brfc {
namespace oh5 {

/**
 * @brief ODIM_H5 source
 *
 * this is a simple key <-> value mapping. Although ODIM_H5 specifies
 * [WMO,RAD,ORG,PLC,CTY,CMT] as valid keys, this class accepts anything.
 */
class Source {
  public:
    /**
     * @brief construct empty Source
     */
    Source()
            : map_() {
    }
    
    /**
     * @brief copy constructor
     */
    Source(const Source& other)
            : map_(other.map_) {
    }
    
    /**
     * @brief copy assignment
     */
    Source& operator=(const Source& rhs) {
        if (this != &rhs) {
            map_ = rhs.map_;
        }
        return *this;
    }

    /**
     * @brief construct from a string
     * @param source key:value pairs separated by comma
     * @throw value_error when source is incorrectly formed
     */
    static Source from_string(const std::string& source);
    
    /**
     * @brief add a new key:value pair
     * @throw duplicate_entry when key already has a value
     */
    void add(const std::string& key, const std::string& value);
    
    /**
     * @brief test if key is present
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief key value
     * @throw lookup_error when key does not exist
     */
    const std::string& get(const std::string& key) const;
    
    /**
     * @brief list of keys (excluding those prefixed with "_")
     * @note in Java this method returns List<String>
     */
    std::vector<std::string> keys() const;

    /**
     * @brief list of all keys (including those prefixed with "_")
     * @note in Java this method returns List<String>
     */
    std::vector<std::string> all_keys() const;
    
    /**
     * @throw lookup_error when key does not exist
     */
    void remove(const std::string& key);
    
    /**
     * @brief clear the contents
     */
    void clear();
    
    /**
     * @brief return true if source is empty
     */
    bool empty() const;
    
    /**
     * @brief join to key:value pairs separated by comma
     *
     * for example, if this source held {"key1": "value1", "key2": "value2"},
     * the returned string would be "key1:value1,key2:value2".
     *
     * @note keys prefixed with "_" are not included in the string
     */
    std::string to_string() const;

  private:
    typedef std::map<std::string, std::string> Map;

    Map map_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_H
