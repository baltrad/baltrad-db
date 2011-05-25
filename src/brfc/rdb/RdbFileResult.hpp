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

#ifndef BRFC_DB_RDB_RDB_FILE_RESULT_HPP
#define BRFC_DB_RDB_RDB_FILE_RESULT_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <brfc/db/FileResult.hpp>

namespace brfc {

    class RelationalDatabase;

    namespace sql {
        class Connection;
        class Result;
    }
}

namespace brfc {

class RdbFileResult : public FileResult {
  public:
    RdbFileResult(RelationalDatabase* rdb,
                  boost::shared_ptr<sql::Connection> conn,
                  sql::Result* result)
            : rdb_(rdb)
            , conn_(conn)
            , result_(result) {
    }

  protected:
    virtual bool do_seek(int pos);
    virtual bool do_next();
    virtual int do_size() const;
    virtual FileEntry* do_entry();
  
  private:
    RelationalDatabase* rdb_;
    boost::shared_ptr<sql::Connection> conn_;
    boost::scoped_ptr<sql::Result> result_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RDB_FILE_RESULT_HPP
