/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_RDB_ID_CACHE_HPP
#define BRFC_RDB_ID_CACHE_HPP

#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/optional.hpp>

namespace brfc {
namespace rdb {

/**
 * @brief key <-> value cache with (optional) key/value lookups
 * 
 * both keys and values contained are unique
 * 
 * @tparam Key must fulfill boost::optional requirements
 * @tparam Value must fulfill boost::optional requirements
 */
template<typename Key, typename Value>
class Cache {
    typedef boost::bimaps::bimap<
                boost::bimaps::set_of<Key>,
                boost::bimaps::set_of<Value>
            > Map;
    
    typedef typename Map::left_iterator left_iterator;
    typedef typename Map::left_const_iterator left_const_iterator;
    typedef typename Map::right_iterator right_iterator;
    typedef typename Map::right_const_iterator right_const_iterator;
  
  public:
    typedef boost::optional<Key> OptionalKey;
    typedef boost::optional<Value> OptionalValue;

    enum LookupPolicy {
        NO_LOOKUP = 0, ///< don't do a lookup
        LOOKUP_MISSING = 1, ///< do a lookup if value is missing
        FORCE_LOOKUP = 2 ///< always do a lookup
    };

    /**
     * @brief construct an empty map
     */
    Cache()
            : map_() {
    }
    
    /**
     * @brief destructor
     */
    virtual ~Cache() { }
    
    /**
     * @brief get key associated with @c value, optionally doing a lookup
     *
     * if @c obj is not cached, query and store key
     */
    OptionalKey key(const Value& value,
                    LookupPolicy lookup=LOOKUP_MISSING) {
        OptionalKey key;
        if (lookup == FORCE_LOOKUP) {
            key = lookup_key(value);
        } else if (lookup == LOOKUP_MISSING) {
            key = stored_key(value);
            if (not key)
                key = lookup_key(value);
        } else {
            key = stored_key(value);
        }
        return key;
    }
    
    /**
     * @brief get value associated with @c key, optionally doing a lookup
     */
    OptionalValue value(const Key& key,
                        LookupPolicy lookup=LOOKUP_MISSING) {
        OptionalValue value;
        if (lookup == FORCE_LOOKUP) {
            value = lookup_value(key);
        } else if (lookup == LOOKUP_MISSING) {
            value = stored_value(key);
            if (not value)
                value = lookup_value(key);
        } else {
            value = stored_value(key);
        }
        return value;
    }
    
    /**
     * @brief get stored key associated with @c value
     */
    OptionalKey stored_key(const Value& value) {
        right_iterator i = map_.right.find(value);
        return i != map_.right.end() ? i->second : OptionalKey();
    }
    
    /**
     * @brief get stored value associated with @c key
     */
    OptionalKey stored_value(const Key& key) {
        left_iterator i = map_.left.find(key);
        return i != map_.left.end() ? i->second : OptionalValue();
    }
    
    /**
     * @brief insert a (@c key, @c value) entry
     *
     * @note if a key or value exists, it will be replaced
     */
    bool insert(const Key& key, const Value& value) {
        typedef typename Map::left_map::value_type kv_type;
        
        // remove key if it exists
        left_iterator ki = map_.left.find(key);
        if (ki != map_.left.end())
            map_.left.erase(ki);
        
        // remove value if it exists
        right_iterator vi = map_.right.find(value);
        if (vi != map_.right.end())
            map_.right.erase(vi);
        
        return map_.left.insert(kv_type(key, value)).second;
    }
    
    /**
     * @brief remove entry by @c value
     * @return true if the entry was removed
     */
    bool remove_by_value(const Value& value) {
        return map_.right.erase(value);
    }
    
    /**
     * @brief remove entry by @c key
     * @return true if the entry was removed
     */
    bool remove_by_key(const Key& key) {
        return map_.left.erase(key);
    }
    
    /**
     * @brief look up key for a @c value
     * @return the key for @c value or empty key if not found
     *
     * if a key for @c value is found, its entry is created/updated
     * if a value for @c key is not found, its entry is removed
     */
    OptionalKey lookup_key(const Value& value) {
        OptionalKey key = do_lookup_key(value);
        if (key)
            insert(key.get(), value);
        else
            remove_by_value(value);
        return key;
    }
    
    /**
     * @brief look up value for a @c key
     * @return the value for @c key or empty value if not found
     *
     * if a value for @c key is found, its entry is created/updated
     * if a value for @c key is not found, its entry is removed
     */
    OptionalValue lookup_value(const Key& key) {
        OptionalValue value = do_lookup_value(key);
        if (value)
            insert(key, value.get());
        else
            remove_by_key(key);
        return value;
    }

    /**
     * @brief remove all entries
     */
    void clear() {
        map_.clear();
    }
    
    /**
     * @brief number of entries in this map
     */
    size_t size() const {
        return map_.size();
    }
    
  protected:
    /**
     * @brief lookup_key() implementation
     * @return empty key
     */
    virtual OptionalKey do_lookup_key(const Value& value) {
        return OptionalKey();
    }
    
    /**
     * @brief lookup_value() implementation
     * @return empty value
     */
    virtual OptionalValue do_lookup_value(const Key& key) {
        return OptionalValue();
    }
    
  private:
    Map map_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_ID_CACHE_HPP
