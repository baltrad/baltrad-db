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
class DataObject;
class File;
class Source;

typedef std::map<QString, QVariant> BindMap;

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
    
  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    virtual void do_remove_file(const char* path);
    virtual void do_save_file(const File& file,
                              const AttributeMapper& mapper);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

    virtual void do_populate_attribute_mapper(AttributeMapper& mapper);

    virtual void do_clean();
  
  private:
    /**
     * @brief save File to database
     */
    id_type save(const File& file,
                 const AttributeMapper& mapper);

    /**
     * @brief save DataObject to database
     */
    id_type save(const DataObject& dobj,
                 const id_type& file_id,
                 const id_type& parent_id,
                 const AttributeMapper& mapper);
    
    /**
     * @brief save Attribute to database
     */
    void save(const Attribute& attr,
              const id_type& dobj_id,
              const AttributeMapper& mapper);

    /**
     * @brief remove file from database
     */
    void remove_file(const char* path);
    
    /**
     * @brief save File recursively
     *
     * saves File and all DataObjects (also recursively)
     */
    void save_recurse(const File& file, const AttributeMapper& mapper);

    /**
     * @brief save DataObject recursively
     *
     * saves DataObject and all Attributes
     */
    id_type save_recurse(const DataObject& dobj,
                         const id_type& file_id,
                         const id_type& parent_id,
                         const AttributeMapper& mapper);
    
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

    boost::scoped_ptr<QSqlDatabase> sql;
};

}

#endif // BRFC_QT_SQL_DATABASE_H
