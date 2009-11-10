#ifndef BRFC_DATABASE_H
#define BRFC_DATABASE_H

#include <string>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class QSqlDatabase;
class QString;
class QVariant;

namespace brfc {

class Attribute;
class AttributeMapper;
class DataObject;
class File;
class ResultSet;
class Source;


typedef std::map<QString, QVariant> BindMap;

/**
 * @brief Database access
 */
class Database : public boost::noncopyable {
  public:
    typedef QVariant id_type;

    /**
     * @brief constructor
     * @param dsn connection  in URI-like syntax: 
     *            <tt> engine://user:password\@host:port/dbname </tt>
     *
     * the only engine currently supported is 'postgresql'
     */
    explicit Database(const std::string& dsn);
    
    /**
     * @brief destructor
     */
    ~Database();
    
    /**
     * @brief start a transaction
     */
    void begin();

    /**
     * @brief roll back current transaction
     */
    void rollback();

    /**
     * @brief commit current transaction
     */
    void commit();
    
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
    
    /**
     * @brief execute a select query
     */
    ResultSet query(const QString& query, const BindMap& binds);
    
    /**
     * @brief query populate AttributeMapper with values from database
     */
    void populate_attribute_mapper(AttributeMapper& mapper);
    
    /**
     * @brief delete all files from database
     */
    void clean();

  private:
    QString qt_engine(const QString& engine) const;

    boost::scoped_ptr<QSqlDatabase> sql;
};

}

#endif // BRFC_DATABASE_H
