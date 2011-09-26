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
#ifndef BRFC_RDB_DEFAULT_QUERY_HPP
#define BRFC_RDB_DEFAULT_QUERY_HPP

#include <vector>

#include <boost/shared_ptr.hpp>

#include <brfc/rdb/RdbSourceManager.hpp>
#include <brfc/sql/Factory.hpp>

namespace brfc {
    namespace sql {
        class Connection;
        class Dialect;
        class Result;
    }
}

namespace brfc {

/**
 * @brief gather & simplify database queries
 */
class RdbDefaultSourceManager : public RdbSourceManager {
  public:
    RdbDefaultSourceManager(boost::shared_ptr<sql::Connection> conn);

    virtual ~RdbDefaultSourceManager();

  private:
    virtual long long do_source_id(const Oh5Source& source);
    virtual Oh5Source do_source_by_id(long long id);
    virtual std::vector<Oh5Source> do_all_sources();
    virtual void do_add_source(const Oh5Source& source);
    virtual void do_update_source(const Oh5Source& source);
    virtual void do_remove_source(const Oh5Source& source);

    sql::Connection& conn();

    const sql::Dialect& dialect();

    long long last_id(sql::Result& result);

    boost::shared_ptr<sql::Connection> conn_;
    sql::Factory sql_;
};

} // namespace brfc

#endif // BRFC_RDB_QUERY_HPP
