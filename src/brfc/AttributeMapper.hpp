#ifndef BRFC_ATTRIBUTE_MAPPER_HPP
#define BRFC_ATTRIBUTE_MAPPER_HPP

#include <boost/noncopyable.hpp>

#include <map>
#include <string>
#include <vector>

namespace brfc {

class Converter;    

/**
 * @brief Attribute mapping to database table/column
 */
struct Mapping {
    Mapping(const std::string& attribute_,
            const std::string& table_,
            const std::string& column_)
            : attribute(attribute_)
            , table(table_)
            , column(column_) {
    }
    
    std::string attribute; ///< attribute name
    std::string table; ///< database table
    std::string column; ///< database column
};

/**
 * @brief vector of Mappings
 */
typedef std::vector<Mapping> MappingVector;


/**
 * @brief Attribute definition in database
 */
struct AttributeSpec {
  public:
    /**
     * @brief constructor
     */
    AttributeSpec(int id_,
                  const std::string& name_,
                  const Converter& converter_,
                  const std::string& table_,
                  const std::string& column_)
            : id(id_)
            , name(name_)
            , converter(converter_)
            , table(table_)
            , column(column_) {
        
    }

    int id; ///< unique id
    std::string name; ///< Attribute name
    const Converter& converter; ///< Converter to use when reading/writing HDF5
    std::string table; ///< table where the value is stored
    std::string column; ///< column in the table where value is stored
};

/**
 * @brief holds AttributeSpec\'s
 */
class AttributeMapper : public boost::noncopyable {
  public:
    /**
     * @brief default constructor
     */
    AttributeMapper();

    /**
     * @brief destructor
     */
    ~AttributeMapper();

    /**
     * @brief add a specification
     *
     * @param id id of this specification
     * @param name name of the Attribute
     * @param converter name of the Conveter
     * @param table name of the table where Attribute value is stored
     * @param column column in the table where Attribute value is stored
     */
    void add_spec(int id,
                  const std::string& name,
                  const std::string& converter,
                  const std::string& table,
                  const std::string& column);
    
    /**
     * @brief get AttributeSpec
     * @param attribute Attribute name
     * @throw lookup_error if not found
     */
    const AttributeSpec&
    spec(const std::string& attribute) const;
     
    /**
     * @brief is Attribute specialized
     * @param attribute Attribute name
     * @return true if attribute is found and is specialized
     * @throw lookup_error if not found
     */
    bool is_specialized(const std::string& attribute) const;
    
    /**
     * @brief get specializations on table
     * @param table table name
     * @return MappingVector for Attributes that have their values stored
               in the table
     */
    MappingVector
    specializations_on(const std::string& table) const;

    /**
     * @brief is mapper aware of Attribute
     * @param attribute Attribute name
     * @return true if found
     */
    bool has(const std::string& attribute) const; 

    /**
     * @brief get Mapping
     * @param attribute Attribute name
     * @return Mapping for the Attribute
     * @throw lookup_error if not found
     */
    Mapping
    mapping(const std::string& attribute) const;
    
    /**
     * @brief get Converter
     * @param attribute Attribute name
     * @return Converter for the Attribute
     * @throw lookup_error if not found
     */
    const Converter& converter(const std::string& attribute) const;

  private:
    typedef std::map<std::string, AttributeSpec> AttributeSpecMap;
    typedef std::map<std::string, Converter*> ConverterMap;

    AttributeSpecMap specs_;
    ConverterMap converters_;
};

} // namespace brfc

#endif //BRFC_ATTRIBUTE_MAPPER_HPP
