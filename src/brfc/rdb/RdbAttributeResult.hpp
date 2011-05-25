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

#ifndef BRFC_DB_RDB_ATTRIBUTE_RESULT_HPP
#define BRFC_DB_RDB_ATTRIBUTE_RESULT_HPP

#include <brfc/db/AttributeResult.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace brfc {
    namespace sql {
        class Connection;
        class Result;
    }
}

namespace brfc {

/**
 * @brief mediate sql::Result to AttributeResult
 */
class RdbAttributeResult : public AttributeResult {
  public:
    /**
     * @param result the sql query result
     * @note ownership of @c result transfers to this
     */
    RdbAttributeResult(boost::shared_ptr<sql::Connection> conn,
                       sql::Result* result)
            : conn_(conn)
            , result_(result) {
    }
  
  protected:
    virtual bool do_next();

    virtual bool do_seek(int idx);

    virtual int do_size() const;

    virtual Variant do_value_at(const std::string& name) const;

  private:
    boost::shared_ptr<sql::Connection> conn_;
    boost::scoped_ptr<sql::Result> result_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_ATTRIBUTE_RESULT_HPP
