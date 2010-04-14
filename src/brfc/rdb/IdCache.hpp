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

#include <map>
#include <utility>

#include <boost/optional.hpp>

#include <brfc/smart_ptr.hpp>

namespace brfc {

namespace rdb {

/**
 * @brief Abstract base class for caching database ids
 * @tparam T must derive from enable_shared_from_this
 * @tparam Id must fulfill boost::optional requirements
 */
template<typename T, typename Id>
class IdCache {
  public:
    typedef std::map<weak_ptr<const T>, Id> IdMap;
    typedef boost::optional<Id> OptionalId;

    virtual ~IdCache() { }

    OptionalId get(const T& obj) {
        weak_ptr<const T> ptr(obj.shared_from_this());

        OptionalId value = get_cached(ptr);
        if (not value) {
            value = query(obj);
            if (value) 
                set_cached(ptr, value.get());
        }
        return value;
    }
    
    /**
     * @note if the pointer is expired and an entry is found, the entry
     *       is removed.
     */
    OptionalId get_cached(weak_ptr<const T> ptr) {
        typename IdMap::iterator i = cache_.find(ptr);
        if (i->first.expired()) {
            remove_cached(ptr);
            i = cache_.end();
        } 
        return i != cache_.end() ? i->second : OptionalId();
    }
    
    /**
     * @throw value_error if ptr is expired or id is null
     */
    void set_cached(weak_ptr<const T> ptr, Id id) {
        if (not ptr.expired())
            cache_[ptr] = id;
    }
    
    bool remove_cached(weak_ptr<const T> ptr) {
        return cache_.erase(ptr);
    }

    OptionalId query(const T& obj) {
        return do_query(obj);
    }

    /**
     * @brief remove all entries
     */
    void clear() {
        cache_.clear();
    }
    
    /**
     * @brief remove entries for expired pointers
     */
    void remove_expired() {
        typename IdMap::iterator iter = cache_.begin();
        for ( ; iter != cache_.end(); ++iter) {
            if (iter->first.expired())
                cache_.erase(iter);
        }
    }

    size_t size() const {   
        return cache_.size();
    }
 
  protected:
    virtual OptionalId do_query(const T& obj) = 0;

  private:
    IdMap cache_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_ID_CACHE_HPP
