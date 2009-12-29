#ifndef BRFC_FILE_H
#define BRFC_FILE_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class QDate;
class QTime;

namespace brfc {

class AttributeMapper;
class DataObject;
class Source;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 *
 * @note Specification states that:
 *       <i>Software is encouraged to warn if it receives a ﬁle
 *       stored in a version which is different from the one it
 *       expects. The software should, however, proceed to read
 *       the ﬁle, ignoring Attributes it does not understand</i>
 *       This library currently silently ignores Attributes it does
 *       not recognize. (in the future, ignored Attributes should
 *       be made accessible through this interface).
 */
class File : public boost::noncopyable {
  public:
    /**
     * @brief construct an empty File
     *
     * this is mainly useful for testing purposes
     */
    File();
    
    /**
     * @brief construct from physical file
     * @param path absolute path to the file
     * @param mapper AttributeMapper instance used for converter lookups
     * @throw fs_error if file can not be opened
     */
    File(const std::string& path, const AttributeMapper& mapper);
    
    /**
     * @brief construct with mandatory attributes present
     * @param object /what/object
     * @param version /what/version
     * @param date /what/date
     * @param time /what/time
     * @param source /what/source
     *
     * this is the minimal "correct" file, given that parameters are
     * correctly formed.
     */
    File(const std::string& object,
         const std::string& version,
         const QDate& date,
         const QTime& time,
         const std::string& source);

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
    DataObject& data_object(const std::string& path,
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
     *
     * construct a Source instance from Attribute 'source' on root DataObject
     */
    Source source() const;

    /**
     * @brief path this File should be stored to
     *
     * retrieve pseudo-attribute \c path on root DataObject
     */
    std::string path() const;

  private:
    /**
     * @brief load from filesystem
     */
    void load(const std::string& path, const AttributeMapper& mapper);

    /**
     * @brief figure out pathname from attributes
     *
     * pseudo-attribute \c path on root DataObject is set to a relative
     * path where this File should be stored to.
     */
    void set_path_from_attributes();

    boost::scoped_ptr<DataObject> root_;
};

} // namespace brfc
#endif // BRFC_FILE_H
