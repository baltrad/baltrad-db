#ifndef BRFC_QSQL_DATABASE_H
#define BRFC_QSQL_DATABASE_H

#include <brfc/Database.hpp>

#include <string>
#include <map>

#include <QtCore/QString>
#include <QtSql/QSqlDatabase>

#include <boost/scoped_ptr.hpp>

class QVariant;
class QCoreApplication;

namespace brfc {

class Attribute;
class AttributeMapper;
class AttributeSpecs;
class DataObject;
class File;
class Source;
class Variant;

typedef std::map<QString, Variant> BindMap;

/**
 * @brief Database using QtSql
 */
class RelationalDatabase : public Database {
  public:
    typedef QVariant id_type;

    /**
     * @brief constructor
     * @param dsn connection  in URI-like syntax: 
     *            <tt> engine://user:password\@host:port/dbname </tt>
     *
     * the only engine currently supported is 'postgresql'
     */
    explicit RelationalDatabase(const QString& dsn);
    
    /**
     * @brief destructor
     */
    virtual ~RelationalDatabase();

    /**
     * @brief execute a select query
     */
    shared_ptr<ResultSet> query(const QString& query,
                                const BindMap& binds) const;
    
    const AttributeSpecs& specs() const;

    const AttributeMapper& mapper() const;

    void populate_mapper_and_specs();

    QSqlDatabase& connection() const {
        RelationalDatabase* self = const_cast<RelationalDatabase*>(this);
        return self->sql_;
    }

    bool supports_returning() const {
        return supports_returning_;
    }

  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();
    
    virtual bool do_has_file(const File& file) const;
    virtual void do_remove_file(const QString& path);
    virtual long long do_save_file(const QString& path, const File& file);

    virtual shared_ptr<Source> do_load_source(const QString& srcstr);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

    virtual void do_clean();

    QString dialect() const { return dialect_; }
  
  private:
    void init();

    void init_qapp();

    /**
     * @brief save File to database
     */
    id_type save(const QString& path, const File& file);

    /**
     * @brief save Attribute to database
     */
    void save(const Attribute& attr,
              const id_type& dobj_id);

    /**
     * @brief remove file from database
     */
    void remove_file(const QString& path);
    
    QString qt_engine(const QString& engine) const;

    QSqlDatabase sql_;
    boost::scoped_ptr<AttributeMapper> mapper_;
    boost::scoped_ptr<AttributeSpecs> specs_;
    QString dialect_;
    bool supports_returning_;
    static unsigned int connection_count_;
    static unsigned int instance_count_;
    static int argc_;
    static const char* argv_[];
    static QCoreApplication* qapp_;
};


}

#endif // BRFC_QT_SQL_DATABASE_H
