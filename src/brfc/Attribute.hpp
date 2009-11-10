#ifndef BRFC_ATTRIBUTE_H
#define BRFC_ATTRIBUTE_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


class QVariant;

namespace brfc {

class DataObject;

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
    Attribute(const std::string& name,
              const QVariant& value,
              const DataObject* data_object=0);
    
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
    const QVariant& value() const { return *value_; }
    
    /**
     * @brief set attribute value
     */
    void value(const QVariant& value);

    /**
     * @brief full path of this attribute
     * full path of the data object + attribute name
     */
    std::string path() const;

    /**
     * @brief name of this attribute
     */
    const std::string& name() const { return name_; }

  private:
    const DataObject* data_object_;
    std::string name_;
    boost::scoped_ptr<QVariant> value_;
};


} // namespace brfc

#endif // BRFC_ATTRIBUTE_H  
