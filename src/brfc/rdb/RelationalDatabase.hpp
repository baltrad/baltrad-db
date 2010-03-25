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

class Variant;

namespace oh5 {

class AttributeSpecs;
class File;
class Source;
class SourceCentre;
class SourceRadar;

}

namespace rdb {

class AttributeMapper;

typedef std::map<QString, Variant> BindMap;
typedef std::map<weak_ptr<void>, long long> IdCache;

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
                                const BindMap& binds);
    
    const oh5::AttributeSpecs& specs() const;

    const AttributeMapper& mapper() const;

    void populate_mapper_and_specs();

    QSqlDatabase& connection() {
        return sql_;
    }

    bool supports_returning() const {
        return supports_returning_;
    }

  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();
    
    virtual bool do_has_file(const oh5::File& file);
    virtual void do_remove_file(const QString& path);
    virtual long long do_save_file(const oh5::File& file);

    virtual shared_ptr<oh5::Source> do_load_source(const QString& srcstr);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

    QString dialect() const { return dialect_; }
  
  private:
    void init();

    void init_qapp();

    void load_source_radar(shared_ptr<oh5::SourceRadar> src);
    void load_source_centre(shared_ptr<oh5::SourceCentre> src);

    /**
     * @brief save file to database
     */
    id_type save(const oh5::File& file);

    /**
     * @brief remove file from database
     */
    void remove_file(const QString& path);
    
    QString qt_engine(const QString& engine) const;

    QSqlDatabase sql_;
    boost::scoped_ptr<AttributeMapper> mapper_;
    boost::scoped_ptr<oh5::AttributeSpecs> specs_;
    QString dialect_;
    bool supports_returning_;
    IdCache id_cache_;
    static unsigned int connection_count_;
    static unsigned int instance_count_;
    static int argc_;
    static const char* argv_[];
    static QCoreApplication* qapp_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_QT_SQL_DATABASE_H
