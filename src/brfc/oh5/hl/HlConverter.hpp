/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_OH5_HL_CONVERTER_HPP
#define BRFC_OH5_HL_CONVERTER_HPP

extern "C" {
    #include <hlhdf_types.h>
}

#include <string>
#include <vector>

#include <brfc/smart_ptr.hpp>

namespace brfc {

class Oh5Scalar;

/**
 * @brief HLHDF scalar data as raw byte array
 */
class HL_Data {
  public:
    /**
     * @brief constructor
     * @param size size of the array (number of bytes in the value)
     * @param type name of the type
     */
    HL_Data(size_t size, const std::string& type, unsigned char* data)
            : type_(type)
            , data_(data, data + size) {
    } 
    
    /**
     * @brief access typename of contained data
     */
    const char* type() const { return type_.c_str(); }
    
    /**
     * @brief access array size
     */
    size_t size() const { return data_.size(); }
    
    /**
     * @brief access array
     */
    unsigned char* data() {
        return &data_[0];
    }

  private:
    std::string type_;
    std::vector<unsigned char> data_;
};

/**
 * @brief ABC for HL_Data <-> Scalar conversion
 */
class HlConverter {
  public:
    static
    shared_ptr<const HlConverter>
    create_from_hlhdf_node(const HL_Node& node);

    static
    shared_ptr<const HlConverter>
    create_from_variant(const Oh5Scalar& variant);

    /**
     * @brief convert data from HLHDF format to Scalar
     * @param format format as defined in HLHDF library
     * @param data value bytes
     * @return Scalar containing the value
     * @throw value_error if the conversion fails
     *
     * @see do_convert(HL_FormatSpecifier, unsigned char*) const
     */
    Oh5Scalar convert(const HL_Node& node) const;
    
    /**
     * @brief convert a Scalar to HLHDF format
     * @param value the Scalar to convert
     * @throw value_error if the conversion fails
     *
     * @see do_convert(const Scalar&) const
     */
    HL_Data convert(const Oh5Scalar& value) const;

    /**
     * @brief destructor
     */
    virtual ~HlConverter() {}

  protected:
    /**
     * @brief do the actual conversion to Scalar
     */
    virtual Oh5Scalar do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const = 0;

    /**
     * @brief do the actial conversion to HL_Data
     */
    virtual HL_Data do_convert(const Oh5Scalar& value) const = 0;
};

/**
 * @brief Conversion from HLHDF_STRING to std::string
 *
 * the string is assumed to be encoded in UTF-8.
 *
 * @note ODIM_H5 specifies that strings can use any encoding, with the
 *       default encoding being ASCII. It states using UNICODE might be
 *       preferrable, but again doesn't give any specific encoding.
 */
class HlStringConverter : public HlConverter {
  protected:
    virtual Oh5Scalar do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;
    
    virtual HL_Data do_convert(const Oh5Scalar& value) const;
};

/**
 * @brief Conversion from HLHDF integer formats to 64-bit integer
 *
 * supports conversion from:
 *  - HLHDF_INT
 *  - HLHDF_LONG
 *  - HLHDF_LLONG
 *
 * @note conversion from Scalar to HLHDF format always results in a
 *       HLHDF_LLONG
 * @note unsigned integers probably need their own converter (to fit a
 *       ullong). ODIM_H5 currently doesn't specify any attributes with
 *       ullong precision (in fact, no unsigned values), although it
 *       leaves a possibility for an attribute value to be encoded as one.
 */
class HlInt64Converter : public HlConverter {
  protected:
    virtual Oh5Scalar do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Oh5Scalar& value) const;
};

/**
 * @brief Conversion from HLHDF floating-point formats to double
 *
 * supports conversion from:
 *  - HLHDF_FLOAT
 *  - HLHDF_DOUBLE
 *  - HLHDF_LDOUBLE (with possible loss of precision)
 *
 * @note conversion from Scalar to HLHDF format always results in a
 *       HLHDF_DOUBLE
 */
class HlDoubleConverter : public HlConverter {
  protected:
    virtual Oh5Scalar do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Oh5Scalar& value) const;
};

} // namespace brfc

#endif // BRFC_OH5_HL_CONVERTER_HPP
