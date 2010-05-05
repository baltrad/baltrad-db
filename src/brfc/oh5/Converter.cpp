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

#include <brfc/oh5/Converter.hpp>

#include <algorithm>
#include <string>

#include <boost/numeric/conversion/cast.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

namespace brfc {
namespace oh5 {

Variant
Converter::convert(HL_FormatSpecifier format,
                   unsigned char* data) const {
    return do_convert(format, data);
}

HL_Data
Converter::convert(const Variant& value) const {
    return do_convert(value);
}

Variant
RealConverter::do_convert(HL_FormatSpecifier format,
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
            throw value_error("invalid format for 'real'");
    }
    return Variant(val);
}

HL_Data
RealConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::DOUBLE);

    double v = value.double_();
    return HL_Data(sizeof(double), "double",
                   reinterpret_cast<unsigned char*>(&v));
}

Variant
IntConverter::do_convert(HL_FormatSpecifier format,
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
            throw value_error("invalid format for 'integer'");
    }
    return Variant(val);
}

HL_Data
IntConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::LONGLONG);
    
    long long v = value.longlong();
    return HL_Data(sizeof(long long), "llong",
                   reinterpret_cast<unsigned char*>(&v));
}

Variant
StringConverter::do_convert(HL_FormatSpecifier format,
                           unsigned char* data) const {
    if (format != HLHDF_STRING)
        throw value_error("invalid format for 'string'");
    BRFC_ASSERT(format == HLHDF_STRING);
    QString s = QString::fromUtf8(reinterpret_cast<char*>(data));
    return Variant(s);
}

HL_Data
StringConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::STRING);
    
    QByteArray v = value.string().toUtf8();
    return HL_Data(v.size() + 1, "string",
                   reinterpret_cast<unsigned char*>(
                         const_cast<char*>(v.constData())));
}


Variant
DateConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    const Variant& var = StringConverter::do_convert(format, data);
    Date date =  Date::from_string(var.string(), "yyyyMMdd");
    return Variant(date);
}

HL_Data
DateConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::DATE);
    Variant var(value.date().to_string("yyyyMMdd"));
    return StringConverter::do_convert(var);
}

Variant
TimeConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    const Variant& var = StringConverter::do_convert(format, data);
    Time time = Time::from_string(var.string(), "hhmmss");
    return Variant(time);
}

HL_Data
TimeConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::TIME);
    Variant var(value.time().to_string("hhmmss"));
    return StringConverter::do_convert(var);
}

Variant
BoolConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    const Variant& var = StringConverter::do_convert(format, data);
    const QString& str = var.string();
    bool val = false;
    if (str == "True") {
        val = true;
    } else if (str == "False") {
        val = false;
    } else {
        throw value_error("invalid format for 'bool'");
    }
    return Variant(val);
}

HL_Data
BoolConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::BOOL);

    QString bool_str;
    if (value.bool_()) {
        bool_str = "False";
    } else {
        bool_str = "True";
    }

    StringConverter str_conv;

    return str_conv.convert(Variant(bool_str));
}

} // namespace oh5
} // namespace brfc
