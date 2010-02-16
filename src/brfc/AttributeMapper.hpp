#ifndef BRFC_ATTRIBUTE_MAPPER_HPP
#define BRFC_ATTRIBUTE_MAPPER_HPP

#include <map>
#include <string>
#include <vector>

namespace brfc {

/**
 * @brief Attribute mapping to database table/column
 */
struct Mapping {
    Mapping(int id_,
            const std::string& attribute_,
            const std::string& table_,
            const std::string& column_)
            : id(id_)
            , attribute(attribute_)
            , table(table_)
            , column(column_) {
    }
    
    int id; ///< unique id
    std::string attribute; ///< Attribute name
    std::string table; ///< table where the value is stored
    std::string column; ///< column in the table where value is stored
};

/**
 * @brief vector of Mappings
 */
typedef std::vector<Mapping> MappingVector;

/**
 * @brief map attributes to relational database (table, column)
 */
class AttributeMapper {
  public:
    /**
     * @brief default constructor
     */
    AttributeMapper();

    /**
     * @brief copy constructor
     */
    AttributeMapper(const AttributeMapper& other);

    /**
     * @brief copy assignment
     */
    AttributeMapper& operator=(const AttributeMapper& rhs);

    /**
     * @brief destructor
     */
    ~AttributeMapper();
    
    /**
     * @brief add a mapping
     *
     * @param mapping column in the table where Attribute value is stored
     * @throw duplicate_entry if a mapping for attribute is already defined
     */
    void add(const Mapping& mapping);
    
    /**
     * @brief is Attribute specialized
     * @param attribute Attribute name
     * @return true if attribute is found and is specialized
     * @throw lookup_error if not found
     *
     * attribute is specialized if it is stored outside the default table
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

    void clear() { mappings_.clear(); }
    
  private:
    typedef std::map<std::string, Mapping> MappingMap;

    MappingMap mappings_;
};

} // namespace brfc

#endif //BRFC_ATTRIBUTE_MAPPER_HPP
