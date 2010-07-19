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

#ifndef BRFC_RDB_RELATIONAL_RESULT_SET_HPP
#define BRFC_RDB_RELATIONAL_RESULT_SET_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/ResultSet.hpp>

namespace brfc {

namespace sql {
    class Result;
}

namespace rdb {

/**
 * @brief mediate rdb::Result to ResultSet
 */
class RelationalResultSet : public ResultSet {
  public:
    explicit RelationalResultSet(shared_ptr<sql::Result> result)
            : result_(result) {
    }
  
  protected:
    virtual bool do_next();

    virtual bool do_seek(int idx);

    virtual int do_size();

    virtual Variant do_value_at(unsigned int pos) const;

  private:
    shared_ptr<sql::Result> result_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_RELATIONAL_RESULT_SET_HPP
