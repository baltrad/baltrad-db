/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_OH5_ATTRIBUTE_SPECS_HPP
#define BRFC_OH5_ATTRIBUTE_SPECS_HPP

#include <brfc/smart_ptr.hpp>

#include <map>

#include <QtCore/QString>

namespace brfc {
namespace oh5 {

class Converter;

struct AttributeSpec {
    AttributeSpec(const QString& name_,
                  const QString& type_,
                  bool ignore_in_hash_=false)
            : name(name_)
            , type(type_)
            , ignore_in_hash(ignore_in_hash_) {
    }

    QString name; ///< name of the Attribute
    QString type; ///< type name of the attribute type
    bool ignore_in_hash; ///< ignore this attribute when hashing metadata
};

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
     * @throw value_error 
     */
    void add(const AttributeSpec& spec);

    bool has(const QString& name) const;

    /**
     * @throw lookup_error if not found
     */
    const AttributeSpec& get(const QString& name) const;

    /**
     * @brief get Converter
     * @param name Attribute name
     * @return Converter for the Attribute
     * @throw lookup_error if not found
     */
    const Converter& converter(const QString& name) const;

    void clear() { specs_.clear(); }

  private:
    typedef std::map<QString, AttributeSpec> AttributeSpecMap;
    typedef std::map<QString, shared_ptr<Converter> > ConverterMap;

    AttributeSpecMap specs_;
    ConverterMap converters_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_SPECS_HPP
