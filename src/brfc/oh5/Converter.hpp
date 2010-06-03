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
 * @brief HLHDF scalar data
 */
class HL_Data {
  public:
    HL_Data(size_t size, const std::string& type, unsigned char* data)
            : type_(type)
            , data_(data, data + size) {
   } 

    const char* type() const { return type_.c_str(); }

    size_t size() const { return data_.size(); }

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
     *
     * @see do_convert
     */
    Variant convert(HL_FormatSpecifier format,
                     unsigned char* data) const;
    
    /**
     * @brief convert QVaraint to HLHDF format
     */
    HL_Data convert(const Variant& value) const;

    /**
     * @brief destructor
     */
    virtual ~Converter() {}

  protected:
    /**
     * @brief do the actual conversion
     */
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const = 0;

    /**
     * @brief do the actial conversion
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
 * the string is assumed to be encoded in UTF-8
 */
class StringConverter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;
    
    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to Date
 */
class DateConverter : public StringConverter {
  protected:
    /**
     * date values in ODIM_H5 files are encoded as YYYYMMDD strings
     */
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to Time
 */
class TimeConverter : public StringConverter {
  protected:
    /**
     * time values in ODIM_H5 files are encoded as HHMMSS strings
     */
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF integer formats to 64-bit integer
 */
class Int64Converter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF floating point formats to double
 */
class DoubleConverter : public Converter {
  protected:
    virtual Variant do_convert(HL_FormatSpecifier format,
                               unsigned char* data) const;

    virtual HL_Data do_convert(const Variant& value) const;
};

/**
 * @brief Conversion from HLHDF_STRING to QLongLong
 */
class BoolConverter : public StringConverter {
  protected:
    /**
     * boolean values in ODIM_H5 files are encoded as "True"/"False" strings
     */
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
