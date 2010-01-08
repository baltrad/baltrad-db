#ifndef BRFC_ATTRIBUTE_SPECS_HPP
#define BRFC_ATTRIBUTE_SPECS_HPP

#include <brfc/smart_ptr.hpp>

#include <map>
#include <string>

namespace brfc {

class Converter;

/**
 * @brief Attribute specifications
 *
 * contains valid attributes and their types
 */
class AttributeSpecs {
  public:
    /**
     * @brief construct empty container
     */
    AttributeSpecs();
    
    /**
     * @brief copy constuctor
     */
    AttributeSpecs(const AttributeSpecs& other);
    
    /**
     * @brief copy assignment
     */
    AttributeSpecs& operator=(const AttributeSpecs& rhs);

    /**
     * @brief destructor
     */
    ~AttributeSpecs();

    /**
     * @brief add a specification
     *
     * @param name name of the Attribute
     * @param type name of the attribute type
     * @throw value_error 
     */
    void add(const std::string& name, const std::string& type);

    bool has(const std::string& name) const;

    /**
     * @brief get Converter
     * @param name Attribute name
     * @return Converter for the Attribute
     * @throw lookup_error if not found
     */
    const Converter& converter(const std::string& name) const;

  private:
    typedef std::map<std::string, std::string> AttributeSpecMap;
    typedef std::map<std::string, shared_ptr<Converter> > ConverterMap;

    AttributeSpecMap specs_;
    ConverterMap converters_;
};

} // namespace brfc

#endif // BRFC_ATTRIBUTE_SPECS_HPP
