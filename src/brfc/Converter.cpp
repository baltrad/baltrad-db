#include <brfc/Converter.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <string>

namespace brfc {

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
            throw brfc_error("invalid format");
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
            throw brfc_error("invalid format");
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
    QDate date =  QDate::fromString(var.string(), "yyyyMMdd");
    BRFC_ASSERT(date.isValid());
    return Variant(date);
}

HL_Data
DateConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::DATE);
    Variant var(value.date().toString("yyyyMMdd"));
    return StringConverter::do_convert(var);
}

Variant
TimeConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    const Variant& var = StringConverter::do_convert(format, data);
    QTime time = QTime::fromString(var.string(), "hhmmss");
    BRFC_ASSERT(time.isValid());
    return Variant(time);
}

HL_Data
TimeConverter::do_convert(const Variant& value) const {
    BRFC_ASSERT(value.type() == Variant::TIME);
    Variant var(value.time().toString("hhmmss"));
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
        BRFC_ASSERT(false);
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

} // namespace brfc
