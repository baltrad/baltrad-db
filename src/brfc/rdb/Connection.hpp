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

#ifndef BRFC_RDB_CONNECTION_HPP
#define BRFC_RDB_CONNECTION_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/rdb/BindMap.hpp>

class QString;

namespace brfc {

class ResultSet;

namespace rdb {

class SqlQuery;

/**
 * @brief ABC for database connections
 */
class Connection {
  public:
    enum Feature {
        RETURNING = 1,
        LAST_INSERT_ID = 2
    };

    virtual ~Connection() {

    }

    /**
     * @brief start a transaction
     * @throw db_error if a transaction is already active
     * @sa do_begin
     */
    void begin();    

    /**
     * @brief rollback current transaction
     * @throw db_error if no active transaction
     * @sa do_rollback
     */
    void rollback();    

    /**
     * @brief commit current transaction
     * @throw db_error if no active transaction
     * @sa do_commit
     */
    void commit();    

    /**
     * @brief test for active transaction
     * @return true if there is a transaction
     * @sa do_in_transaction
     */
    bool in_transaction() {
        return do_in_transaction();
    }

    /**
     * @brief execute an SQL statement
     * @param statement statment to execute
     * @param binds binds to replace into the statement
     *
     * If binds are present, they are replaced using replace_binds().
     *
     * The actual statement (with bind placeholders replaced with values) is
     * executed using do_execute().
     *
     * If no transaction is in progress, executes the statement in a "private"
     * transaction, otherwise the statement executed as part of the ongoing
     * transaction.
     *
     * @sa do_execute
     */
    shared_ptr<ResultSet> execute(const QString& statement,
                                  const BindMap& binds=BindMap());
    
    /**
     * @brief execute a query
     * 
     * equivalent to:
     * @code execute(query.statement(), query.binds()); @endcode
     */
    shared_ptr<ResultSet> execute(const SqlQuery& query);

    /**
     * @brief replace bind placeholders in an SQL statement
     * @throw value_error if not all binds consumed or available
     */
    QString replace_binds(const QString& statement, const BindMap& binds);
    
    /**
     * @sa do_variant_to_string
     */
    QString variant_to_string(const Variant& value) {
        return do_variant_to_string(value);
    }

    bool has_feature(Feature feature) const {
        return do_has_feature(feature);
    }

  protected:
    virtual void do_begin() = 0;
    virtual void do_commit() = 0;
    virtual void do_rollback() = 0;

    virtual shared_ptr<ResultSet> do_execute(const QString& statement) = 0;
    
    /**
     * @return true if there is an ongoing transaction
     */
    virtual bool do_in_transaction() = 0;
    
    /**
     * @return false
     */
    virtual bool do_has_feature(Feature /*feature*/) const {
        return false;
    }
    
    /**
     * @brief default implementation
     * 
     * - date and time in ISO 8601 format
     * - bool values as TRUE or FALSE
     * - none as NULL
     * - string surrounded by apostrophes (')
     */
    virtual QString do_variant_to_string(const Variant& value);
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_CONNECTION_HPP
