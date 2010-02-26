#ifndef BRFC_ATTRIBUTE_H
#define BRFC_ATTRIBUTE_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <QtCore/QString>

namespace brfc {

class DataObject;
class Variant;

/**
 * @brief Attribute read from HDF5 file
 */
class Attribute : public boost::noncopyable {
  public:
    /**
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     * @param data_object owning DataObject
     */
    Attribute(const QString& name,
              const Variant& value,
              const DataObject* data_object=0,
              bool ignore_in_hash=false);
    
    /**
     * @brief destructor
     */
    ~Attribute();
    
    /**
     * @brief data object this attribute is associated with
     */
    const DataObject* data_object() const { return data_object_; }
    
    /**
     * @brief attribute value
     */
    const Variant& value() const { return *value_; }
    
    /**
     * @brief set attribute value
     */
    void value(const Variant& value);

    bool ignore_in_hash() const { return ignore_in_hash_; }

    void ignore_in_hash(bool ignore_in_hash) {
        ignore_in_hash_ = ignore_in_hash;
    }

    /**
     * @brief full path of this attribute
     *
     * full path of the data object + attribute name
     *
     * when the attribute is not associated with a DataObject, just the name
     */
    QString path() const;

    /**
     * @brief name of this attribute
     */
    const QString& name() const { return name_; }
    
    /**
     * @brief string representation of this attribute
     *
     * /path/to/attribute=value
     */
    QString to_string() const;

  private:
    const DataObject* data_object_;
    QString name_;
    boost::scoped_ptr<Variant> value_;
    bool ignore_in_hash_;
};


} // namespace brfc

#endif // BRFC_ATTRIBUTE_H  
