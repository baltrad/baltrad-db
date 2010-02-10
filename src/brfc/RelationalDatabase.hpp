#ifndef BRFC_QSQL_DATABASE_H
#define BRFC_QSQL_DATABASE_H

#include <brfc/Database.hpp>

#include <string>
#include <map>

#include <boost/scoped_ptr.hpp>

class QSqlDatabase;
class QString;
class QVariant;

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
    explicit RelationalDatabase(const std::string& dsn);
    
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

  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();
    
    virtual bool do_has_file(const File& file) const;
    virtual void do_remove_file(const char* path);
    virtual long long do_save_file(const char* path, const File& file);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

    virtual void do_clean();
  
  private:
    void init();

    /**
     * @brief save File to database
     */
    id_type save(const char* path, const File& file);

    /**
     * @brief save DataObject to database
     */
    id_type save(const DataObject& dobj,
                 const id_type& file_id,
                 const id_type& parent_id);
    
    /**
     * @brief save Attribute to database
     */
    void save(const Attribute& attr,
              const id_type& dobj_id);

    /**
     * @brief remove file from database
     */
    void remove_file(const char* path);
    
    /**
     * @brief save File recursively
     *
     * saves File and all DataObjects (also recursively)
     */
    long long save_recurse(const char* path, const File& file);

    /**
     * @brief save DataObject recursively
     *
     * saves DataObject and all Attributes
     */
    id_type save_recurse(const DataObject& dobj,
                         const id_type& file_id,
                         const id_type& parent_id);
    
    /**
     * @brief query database id of a File
     */
    id_type query_id(const File& file);
    
    /**
     * @brief query database id of a DataObject
     */
    id_type query_id(const DataObject& dobj);

    /**
     * @brief query database id of a Source
     */
    id_type query_id(const Source& src);
        
    QString qt_engine(const QString& engine) const;

    void populate_mapper_and_specs();

    boost::scoped_ptr<QSqlDatabase> sql;
    boost::scoped_ptr<AttributeMapper> mapper_;
    boost::scoped_ptr<AttributeSpecs> specs_;
};

}

#endif // BRFC_QT_SQL_DATABASE_H
