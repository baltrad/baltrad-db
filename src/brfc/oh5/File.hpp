#ifndef BRFC_FILE_H
#define BRFC_FILE_H

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

#include <QtCore/QString>

class QDate;
class QTime;

namespace brfc {

namespace oh5 {

class AttributeSpecs;
class Group;
class Root;
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
    static shared_ptr<File> create();

    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @param specs AttributeSpecs instance used for converter lookups
     * @throw fs_error if file can not be opened
     *
     * full paths of ignored attributes (not defined in specs) are stored
     * and accessible through ignored_attributes();
     */
    static shared_ptr<File>
    from_filesystem(const QString& path, const AttributeSpecs& specs);
    
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
    static shared_ptr<File>
    minimal(const QString& object,
            const QDate& date,
            const QTime& time,
            const QString& source,
            const QString& version=QString::fromUtf8("H5rad 2.0"));

    /**
     * @brief destructor
     */
    ~File();

    /**
     * @brief get hold of Group at path
     * @param path path to Group, where elements are separated by '/'
     * @return pointer to requested Group if found
     *
     * group("path/to/group")
     * @{
     */
    shared_ptr<const Group> group(const QString& path) const;

    shared_ptr<Group> group(const QString& path);
    ///@}
    
    /**
     * @brief get the root group
     * @{
     */
    shared_ptr<const Root> root() const { return root_; }

    shared_ptr<Root> root() { return root_; }
    ///@}
    
    /**
     * @brief get the Source definiton
     * @{
     */
    shared_ptr<const Source> source() const {
        return source_;
    }

    shared_ptr<Source> source() {
        return source_;
    }
    ///@}

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
    StringVector ignored_attributes() const;

    /**
     * @{
     */
    const QString& path() const {
        return path_;
    }

    void path(const QString& path) {
        path_ = path;
    }
    ///@}
    
    /**
     * @{
     */
    long long db_id() const { return db_id_; }

    void db_id(long long db_id) const { db_id_ = db_id; }
    ///@}
    
    /**
     * @name mandatory attribute access shorthands
     * @{
     */

    /**
     * @brief access /what/object
     *
     * equivalent to
     * @c root()->child_attribute("what/object")->value().string()
     */
    QString what_object() const;

    /**
     * @brief access /what/date
     *
     * equivalent to
     * @c root()->child_attribute("what/date")->value().date()
     */
    QDate what_date() const;

    /**
     * @brief access /what/time
     *
     * equivalent to
     * @c root()->child_attribute("what/time")->value().time()
     */
    QTime what_time() const;

    /**
     * @brief access /what/source
     *
     * equivalent to
     * @c root()->child_attribute("what/source")->value().string()
     */
    QString what_source() const;
    ///@}

  protected:
    File();
    
  private:

    shared_ptr<Root> root_;
    QString path_;
    shared_ptr<Source> source_;
    mutable long long db_id_;
};

} // namespace oh5
} // namespace brfc
#endif // BRFC_FILE_H
