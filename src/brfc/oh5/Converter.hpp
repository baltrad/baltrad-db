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

#ifndef BRFC_OH5_CONVERTER_HPP
#define BRFC_OH5_CONVERTER_HPP

extern "C" {
    #include <hlhdf_types.h>
}

#include <string>
#include <vector>

namespace brfc {

class Variant;

namespace oh5 {

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
 * @brief ABC for HDF5 data to Variant conversion
 */
class Converter {
  public:
    /**
     * @brief convert data from HLHDF format to Variant
     * @param format format as defined in HLHDF library
     * @param data value bytes
     * @return Variant containing the value
     * @throw value_error if the conversion fails
     *
     * @see do_convert(HL_FormatSpecifier, unsigned char*) const
     */
    Variant convert(HL_FormatSpecifier format,
                     unsigned char* data) const;
    
    /**
     * @brief convert a Variant to HLHDF format
     * @param value the Variant to convert
     * @see do_convert(const Variant&) const
     * @throw value_error if the conversion fails
     */
    HL_Data convert(const Variant& value) const;

    /**
     * @brief destructor
     */
    virtual ~Converter() {}

  protected:
    /**
     * @brief do the actual conversion to Variant
     */
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const = 0;

    /**
     * @brief do the actial conversion to HL_Data
     */
    virtual HL_Data do_convert(const Variant& value) const = 0;
};

/**
 * @defgroup converter Converters
 * @{
 */
/**
 * @brief Conversion from HLHDF_STRING to String
 *
 * the string is assumed to be encoded in UTF-8.
 *
 * @note ODIM_H5 specifies that strings can use any encoding, with the
 *       default encoding being ASCII. It states using UNICODE might be
 *       preferrable, but again doesn't give any specific encoding.
 */
class StringConverter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;
    
    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to Date
 *
 * date is converted from string encoded as YYYYMMDD
 */
class DateConverter : public StringConverter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to Time
 *
 * time is converted from string encoded as HHMMSS
 */
class TimeConverter : public StringConverter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF integer formats to 64-bit integer
 *
 * supports conversion from:
 *  - HLHDF_INT
 *  - HLHDF_LONG
 *  - HLHDF_LLONG
 *
 * @note conversion from Variant to HLHDF format always results in a
 *       HLHDF_LLONG
 * @note unsigned integers probably need their own converter (to fit a
 *       ullong). ODIM_H5 currently doesn't specify any attributes with
 *       ullong precision (in fact, no unsigned values), although it
 *       leaves a possibility for an attribute value to be encoded as one.
 */
class Int64Converter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF floating-point formats to double
 *
 * supports conversion from:
 *  - HLHDF_FLOAT
 *  - HLHDF_DOUBLE
 *  - HLHDF_LDOUBLE (with possible loss of precision)
 *
 * @note conversion from Variant to HLHDF format always results in a
 *       HLHDF_DOUBLE
 */
class DoubleConverter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to bool
 *
 * bool value is converted from a string encoded as "True" or "False"
 */
class BoolConverter : public StringConverter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to String
 * 
 * a sequence in ODIM_H5 files is a string of scalar values separated by
 * commas
 *
 * this converter currently does nothing, the values are returned as strings
 */
class SequenceConverter : public StringConverter {

};

//@}

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_CONVERTER_HPP
