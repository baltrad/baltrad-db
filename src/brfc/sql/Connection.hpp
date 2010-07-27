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

#ifndef BRFC_SQL_CONNECTION_HPP
#define BRFC_SQL_CONNECTION_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Compiler.hpp>

namespace brfc {

class String;

namespace sql {

class Compiler;
class Insert;
class Query;
class Result;
class Select;

/**
 * @brief ABC for database connections
 */
class Connection {
  public:
    enum Feature {
        RETURNING = 1,
        LAST_INSERT_ID = 2
    };

    Connection(shared_ptr<Compiler> compiler=shared_ptr<Compiler>());

    /**
     * @brief create a Connection instance from dsn
     * @param dsn dsn (transformed to Url)
     * @throw value_error if url with invalid scheme is supplied
     *
     * construct a connection according to Url::scheme.
     *
     * schemes map as:
     *  - postgresql -> PostgresConnection
     */
    static shared_ptr<Connection> create(const String& dsn);

    virtual ~Connection();    

    /**
     * @brief open the connection
     * @throw db_error if connection is already open
     * @sa do_open
     */
    void open();
    
    /**
     * @brief test for open connection
     * @sa do_is_open
     */
    bool is_open() const {
        return do_is_open();
    }
    
    /**
     * @brief close the connection
     * @throw db_error if connection is not open
     * @sa do_close
     */
    void close();

    /**
     * @brief start a transaction
     * @throw db_error if no conenction open or a transaction is already
     *        active
     *
     * a check for open connection and no active transaction is made prior
     * to forwading the call to do_begin()
     * @sa do_begin
     */
    void begin();    

    /**
     * @brief rollback current transaction
     * @throw db_error if no connection open or no active transaction
     *
     * a check for open connection and active transaction is made prior
     * to forwading the call to do_rollback()
     * @sa do_rollback
     */
    void rollback();    

    /**
     * @brief commit current transaction
     * @throw db_error if no connection open or no active transaction
     *
     * a check for open connection and active transaction is made prior
     * to forwading the call to do_commit()
     * @sa do_commit
     */
    void commit();

    /**
     * @brief test for active transaction
     * @return true if there is a transaction
     * @sa do_in_transaction
     */
    bool in_transaction() const {
        return do_in_transaction();
    }

    /**
     * @brief execute a statement
     *
     * @param statement statment to execute
     * @param binds binds to replace into the statement
     * 
     * equivalent to:
     * @code execute(Query(statement, binds)); @endcode
     */
    shared_ptr<Result> execute(const String& statement,
                               const BindMap& binds=BindMap());
    
    /**
     * @brief execute an Insert statement
     */
    shared_ptr<Result> execute(const Insert& stmt);

    /**
     * @brief execute a Select statement
     */
    shared_ptr<Result> execute(const Select& stmt);
    
    /**
     * @brief execute an SQL query
     *
     * The actual statement (returned by SqlQuery::replace_binds(*this)) is
     * executed using do_execute().
     *
     * If no transaction is in progress, executes the statement in a "private"
     * transaction, otherwise the statement executed as part of the ongoing
     * transaction.
     *
     * @sa do_execute
     */
    shared_ptr<Result> execute(const Query& query);

    /**
     * @sa do_variant_to_string
     */
    String variant_to_string(const Variant& value) const {
        return do_variant_to_string(value);
    }

    bool has_feature(Feature feature) const {
        return do_has_feature(feature);
    }

    String dialect() const {
        return do_dialect();
    }

  protected:
    virtual void do_open() = 0;
    virtual void do_close() = 0;
    virtual bool do_is_open() const = 0;

    virtual void do_begin() = 0;
    virtual void do_commit() = 0;
    virtual void do_rollback() = 0;

    virtual shared_ptr<Result> do_execute(const String& statement) = 0;
    
    /**
     * @return true if there is an ongoing transaction
     */
    virtual bool do_in_transaction() const = 0;
    
    /**
     * @return false
     */
    virtual bool do_has_feature(Feature /*feature*/) const {
        return false;
    }

    virtual String do_dialect() const = 0;
    
    /**
     * @brief default implementation
     * 
     * - date and time in ISO 8601 format
     * - bool values as TRUE or FALSE
     * - none as NULL
     * - string surrounded by apostrophes (')
     */
    virtual String do_variant_to_string(const Variant& value) const;
  
  private:
    shared_ptr<Compiler> compiler_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_CONNECTION_HPP
