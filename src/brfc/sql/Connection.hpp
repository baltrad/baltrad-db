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

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

#include <brfc/sql/BindMap.hpp>
#include <brfc/sql/Compiler.hpp>

namespace brfc {

class String;

namespace sql {

class Compiler;
class Dialect;
class Insert;
class LargeObject;
class Query;
class Result;
class Select;

/**
 * @brief ABC for database connections
 */
class Connection : boost::noncopyable {
  public:
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

    shared_ptr<Result> execute(const String& query);

    const Dialect& dialect() const {
        return do_dialect();
    }

    Compiler& compiler() {
        return do_compiler();
    }
    
    /**
     * @brief access large object identified by @c id
     */
    shared_ptr<LargeObject> large_object(long long id);
    
    /**
     * @brief create a new large object from file @c path
     */
    shared_ptr<LargeObject> large_object(const String& path);

    long long last_insert_id() const {
        return do_last_insert_id();
    }

  protected:
    // allow access to protected virtuals
    friend class ConnectionProxy;
    
    /**
     * @brief open() implementation
     */
    virtual void do_open() = 0;

    /**
     * @brief close() implementation
     */
    virtual void do_close() = 0;

    /**
     * @brief is_open() implementation
     */
    virtual bool do_is_open() const = 0;
    
    /**
     * @brief begin() implementation
     */
    virtual void do_begin() = 0;

    /**
     * @brief commit() implementation
     */
    virtual void do_commit() = 0;

    /**
     * @brief rollback() implementation
     */
    virtual void do_rollback() = 0;

    /**
     * @brief in_transaction() implementation
     */
    virtual bool do_in_transaction() const = 0;
    
    /**
     * @brief execute(const Sting& implementation)
     */
    virtual shared_ptr<Result> do_execute(const String& query) = 0;
    
    /**
     * @brief dialect() implementation
     */
    virtual const Dialect& do_dialect() const = 0;
    
    /**
     * @brief compiler() implementation
     */
    virtual Compiler& do_compiler() = 0;
    
    /**
     * @brief large_object(long long) implementation
     */
    virtual shared_ptr<LargeObject> do_large_object(long long id) = 0;

    /**
     * @brief large_object(const String&) implementation
     */
    virtual shared_ptr<LargeObject> do_large_object(const String& path) = 0;
    
    /**
     * @brief last_insert_id() implementation
     */
    virtual long long do_last_insert_id() const = 0;
  
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_CONNECTION_HPP
