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

#ifndef BRFC_SQL_RESULT_PROXY_HPP
#define BRFC_SQL_RESULT_PROXY_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/sql/Result.hpp>

namespace brfc {
namespace sql {

class Connection;

/**
 * @brief a proxy for Result
 */
class ResultProxy : public Result {
  public:
    /**
     * @brief constructor
     * @param conn the database connection
     * @param result the result to proxy
     */
    ResultProxy(shared_ptr<Connection> conn,
                shared_ptr<Result> result);
    
    virtual ~ResultProxy();

  protected: 
    /**
     * @brief forward the call to the proxied result
     */
    virtual bool do_next();

    /**
     * @brief forward the call to the proxied result
     */
    virtual bool do_seek(int idx);

    /**
     * @brief forward the call to the proxied result
     */
    virtual int do_size() const;

    /**
     * @brief forward the call to the proxied result
     */
    virtual Variant do_value_at(unsigned int pos) const;
    
    /**
     * @brief forward the call to the proxied result
     */
    virtual Variant do_value_at(const std::string& pos) const;

    /**
     * @brief forward the call to the proxied result
     */
    virtual int do_affected_rows() const;

  private:
    shared_ptr<Connection> conn_;
    shared_ptr<Result> result_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_RESULT_PROXY_HPP
