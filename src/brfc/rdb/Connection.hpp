#ifndef BRFC_RDB_CONNECTION_HPP
#define BRFC_RDB_CONNECTION_HPP

#include <brfc/smart_ptr.hpp>

#include <brfc/rdb/BindMap.hpp>

class QString;

namespace brfc {

class ResultSet;

namespace rdb {

/**
 * @brief ABC for database connections
 */
class Connection {
  public:
    virtual ~Connection() {

    }

    /**
     * @brief start a transaction
     * 
     * @sa do_begin
     */
    void begin() {
        do_begin();
    }
    
    /**
     * @brief rollback current transaction
     *
     * @sa do_rollback
     */
    void rollback() {
        do_rollback();
    }
    
    /**
     * @brief commit current transaction
     *
     * @sa do_commit
     */
    void commit() {
        do_commit();
    }

    /**
     * @brief execute a query
     * @param query the query string
     * @param binds binds to replace into the query string
     *
     * If binds are present, they are replaced using replace_binds().
     *
     * The actual query (with bind placeholders replaced with values) is
     * executed using do_execute().
     *
     * If no transaction is in progress, executes the query in a "private"
     * transaction, otherwise the query executed as part of the ongoing
     * transaction.
     *
     * @sa do_execute
     */
    shared_ptr<ResultSet> execute(const QString& query,
                                  const BindMap& binds=BindMap());
    
    /**
     * @brief replace bind placeholders in query string
     * @throw value_error if not all binds consumed or available
     */
    QString replace_binds(const QString& query, const BindMap& binds);
    
    /**
     * @sa do_variant_to_string
     */
    QString variant_to_string(const Variant& value) {
        return do_variant_to_string(value);
    }

  protected:
    virtual void do_begin() = 0;
    virtual void do_commit() = 0;
    virtual void do_rollback() = 0;

    virtual shared_ptr<ResultSet> do_execute(const QString& query) = 0;
    
    /**
     * @return true if there is an ongoing transaction
     */
    virtual bool in_transaction() = 0;
    
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
