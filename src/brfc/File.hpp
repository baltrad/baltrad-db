#ifndef BRFC_FILE_H
#define BRFC_FILE_H

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/hlhdf.hpp>
#include <brfc/smart_ptr.hpp>

#include <QtCore/QString>

class QDate;
class QTime;

namespace brfc {

class AttributeSpecs;
class DataObject;
class Source;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 * @ingroup exposed_in_binds
 */
class File : public boost::noncopyable {
  public:
    typedef std::vector<QString> StringVector;

    /**
     * @brief construct an empty File
     *
     * this is mainly useful for testing purposes
     */
    File();
    
    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @param specs AttributeSpecs instance used for converter lookups
     * @throw fs_error if file can not be opened
     *
     * full paths of ignored attributes (not defined in specs) are stored
     * and accessible through ignored_attributes();
     */
    File(const QString& path, const AttributeSpecs& specs);
    
    /**
     * @brief construct with mandatory attributes present
     * @param object /what/object
     * @param date /what/date
     * @param time /what/time
     * @param source /what/source
     * @param version /what/version
     *
     * this is the minimal "correct" file, given that parameters are
     * correctly formed.
     */
    File(const QString& object,
         const QDate& date,
         const QTime& time,
         const QString& source,
         const QString& version=QString::fromUtf8("H5rad 2.0"));

    /**
     * @brief destructor
     */
    ~File();

    /**
     * @brief get hold of DataObject at path
     * @param path path to DataObject, where elements are separated by '/'
     * @param create_missing should missing DataObjects on path be created
     * @return reference to requested DataObject
     *
     * data_object("/path/to/dataobject")
     */
    DataObject& data_object(const QString& path,
                            bool create_missing=false); 
    
    /**
     * @brief get root DataObject
     * 
     * equivalent to: \code data_object("/") \endcode
     */
    const DataObject& root() const { return *root_; }

    /**
     * @brief get root DataObject
     */
    DataObject& root() { return *root_; }
    
    /**
     * @brief get the Source definiton
     */
    shared_ptr<Source> source() const;

    void source(shared_ptr<Source> src);

    /**
     * @brief string that uniquely identifies the file content
     *
     * SHA-1 hash of concatenated string representation of all attributes
     * not marked to be ignored in hashing plus node_id of the associated
     * Source.
     *
     * @note changes to this algorithm most probably trigger the need to
     *       re-evaluate all files in the database
     */
    QString unique_identifier() const;

    /**
     * @brief get attributes ignored on loading
     */
    const StringVector& ignored_attributes() const {
        return ignored_attributes_;
    }

    const QString& path() const {
        return path_;
    }

    void path(const QString& path) {
        path_ = path;
    }

    long long db_id() const { return db_id_; }

    void db_id(long long db_id) const { db_id_ = db_id; }
    
    /**
     * @name mandatory attribute access shorthands
     * @{
     */

    /**
     * @brief access /what/object
     *
     * equivalent to @c root().attribute("what/object").value().string()
     */
    QString what_object() const;

    /**
     * @brief access /what/date
     *
     * equivalent to @c root().attribute("what/date").value().date()
     */
    QDate what_date() const;

    /**
     * @brief access /what/time
     *
     * equivalent to @c root().attribute("what/time").value().time()
     */
    QTime what_time() const;

    /**
     * @brief access /what/source
     *
     * equivalent to @c root().attribute("what/source").value().string()
     */
    QString what_source() const;
    ///@}

  private:
    /**
     * @brief load from filesystem
     */
    void load(const QString& path, const AttributeSpecs& specs);

    void add_attribute_from_node(HL_Node* node,
                                 const AttributeSpecs& specs);

    /**
     * @brief figure out pathname from attributes
     *
     * pseudo-attribute \c path on root DataObject is set to a relative
     * path where this File should be stored to.
     */
    void set_path_from_attributes();

    boost::scoped_ptr<DataObject> root_;
    StringVector ignored_attributes_;
    QString path_;
    shared_ptr<Source> source_;
    mutable long long db_id_;
};

} // namespace brfc
#endif // BRFC_FILE_H
