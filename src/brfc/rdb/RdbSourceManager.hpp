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
#ifndef BRFC_RDB_SOURCE_MANAGER_HPP
#define BRFC_RDB_SOURCE_MANAGER_HPP

#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/oh5/Oh5Source.hpp>

namespace brfc {

/**
 * @brief manage source entries in database
 */
class RdbSourceManager : boost::noncopyable {
  public:
    virtual ~RdbSourceManager() = 0;

    /**
     * @brief lookup id for @c source
     * @return database id of the source or 0 if source is not found
     *
     * m::source_kvs is queryied for key/value pairs defined in source.
     * A source is considered found when exactly one database id is found.
     */
    long long source_id(const Oh5Source& source) {
        return do_source_id(source);
    }
        
    /**
     * @brief load source by database id
     * @return source with all key/value pairs defined in database or
     *         empty source if not found
     */
    Oh5Source source_by_id(long long id) {
        return do_source_by_id(id);
    }

    std::vector<Oh5Source> all_sources() {
        return do_all_sources();
    }

    void add_source(const Oh5Source& source) {
        do_add_source(source);
    }

    void update_source(const Oh5Source& source) {
        do_update_source(source);
    }

    void remove_source(const Oh5Source& source) {
        do_remove_source(source);
    }

  private:
    virtual long long do_source_id(const Oh5Source& source) = 0;
    virtual Oh5Source do_source_by_id(long long id) = 0;
    virtual std::vector<Oh5Source> do_all_sources() = 0;
    virtual void do_add_source(const Oh5Source& source) = 0;
    virtual void do_update_source(const Oh5Source& source) = 0;
    virtual void do_remove_source(const Oh5Source& source) = 0;
};

inline
RdbSourceManager::~RdbSourceManager() { }

} // namespace brfc

#endif // BRFC_RDB_QUERY_HPP
