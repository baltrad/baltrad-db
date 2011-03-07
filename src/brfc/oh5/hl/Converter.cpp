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

#include <brfc/oh5/hl/Converter.hpp>

#include <string>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>

#include <utf8/utf8.h>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/hl/hlhdf.hpp>

#include <iostream>

namespace brfc {
namespace oh5 {
namespace hl {

shared_ptr<const Converter>
Converter::create_from_hlhdf_node(const HL_Node& node) {
    HL_Node* node_ptr = const_cast<HL_Node*>(&node);
    HL_FormatSpecifier fmt = HLNode_getFormat(node_ptr);
    switch (fmt) {
        case HLHDF_INT:
        case HLHDF_LONG:
        case HLHDF_LLONG:
            return make_shared<Int64Converter>();
        case HLHDF_FLOAT:
        case HLHDF_DOUBLE:
        case HLHDF_LDOUBLE:
            return make_shared<DoubleConverter>();
        case HLHDF_STRING:
            return make_shared<StringConverter>();
        default:
            return shared_ptr<Converter>();
    }
}

shared_ptr<const Converter>
Converter::create_from_variant(const Scalar& variant) {
    switch (variant.type()) {
        case Scalar::INT64:
            return make_shared<Int64Converter>();
        case Scalar::DOUBLE:
            return make_shared<DoubleConverter>();
        case Scalar::STRING:
            return make_shared<StringConverter>();
        default:
            return shared_ptr<Converter>();
    }
}

Scalar
Converter::convert(const HL_Node& node) const {
    HL_Node* node_ptr = const_cast<HL_Node*>(&node);
    HL_FormatSpecifier fmt = HLNode_getFormat(node_ptr);
    unsigned char* data = HLNode_getData(node_ptr);
    return do_convert(fmt, data);
}

HL_Data
Converter::convert(const Scalar& value) const {
    return do_convert(value);
}

Scalar
DoubleConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    using boost::numeric_cast;
    double val;
    switch (format) {
        case HLHDF_FLOAT:
            val = numeric_cast<double>(*reinterpret_cast<float*>(data));
            break;
        case HLHDF_DOUBLE:
            val = numeric_cast<double>(*reinterpret_cast<double*>(data));
            break;
        case HLHDF_LDOUBLE:
            //XXX: losing precision here
            val = numeric_cast<double>(*reinterpret_cast<long double*>(data));
            break;
        default:
            throw value_error("invalid format for conversion to double");
    }
    return Scalar(val);
}

HL_Data
DoubleConverter::do_convert(const Scalar& value) const {
    if (value.type() != Scalar::DOUBLE)
        throw value_error("invalid Scalar conversion to HLHDF double");

    double v = value.double_();
    return HL_Data(sizeof(double), "double",
                   reinterpret_cast<unsigned char*>(&v));
}

Scalar
Int64Converter::do_convert(HL_FormatSpecifier format,
                         unsigned char* data) const {
    using boost::numeric_cast;
    long long val;
    switch (format) {
        case HLHDF_INT:
            val = numeric_cast<long long>(*reinterpret_cast<int*>(data));
            break;
        case HLHDF_LONG:
            val = numeric_cast<long long>(*reinterpret_cast<int32_t*>(data));
            break;
        case HLHDF_LLONG:
            val = *reinterpret_cast<long long*>(data);
            break;
        default:
            throw value_error("invalid format for conversion to int64");
    }
    return Scalar(val);
}

HL_Data
Int64Converter::do_convert(const Scalar& value) const {
    if (value.type() != Scalar::INT64)
        throw value_error("invalid Scalar conversion to HLHDF llong");
    
    long long v = value.int64_();
    return HL_Data(sizeof(long long), "llong",
                   reinterpret_cast<unsigned char*>(&v));
}

Scalar
StringConverter::do_convert(HL_FormatSpecifier format,
                           unsigned char* data) const {
    if (format != HLHDF_STRING)
        throw value_error("invalid format for conversion to std::string");
    std::string s(reinterpret_cast<char*>(data));
    std::string::iterator end = s.end();
    if (utf8::find_invalid(s.begin(), s.end()) != s.end())
        throw value_error("invalid utf-8: " + s);
    return Scalar(s);
}

HL_Data
StringConverter::do_convert(const Scalar& value) const {
    if (value.type() != Scalar::STRING)
        throw value_error("invalid Scalar conversion to HLHDF string");
    std::string v = value.string();
    return HL_Data(v.size() + 1, "string",
                   reinterpret_cast<unsigned char*>(
                         const_cast<char*>(v.c_str())));
}

} // namespace hl
} // namespace oh5
} // namespace brfc
