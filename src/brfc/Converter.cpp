#include <brfc/Converter.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <QtCore/QVariant>
#include <QtCore/QDate>
#include <QtCore/QTime>

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>
#include <string>

namespace brfc {

QVariant
Converter::convert(HL_FormatSpecifier format,
                   unsigned char* data) const {
    return do_convert(format, data);
}

HL_Data
Converter::convert(const QVariant& value) const {
    return do_convert(value);
}

QVariant
RealConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    using boost::numeric_cast;
    QVariant val;
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
    return val;
}

HL_Data
RealConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::Double);

    double v = value.toDouble();
    return HL_Data(sizeof(double), "double",
                   reinterpret_cast<unsigned char*>(&v));
}

QVariant
IntConverter::do_convert(HL_FormatSpecifier format,
                         unsigned char* data) const {
    using boost::numeric_cast;
    QVariant val;
    switch (format) {
        case HLHDF_INT:
            val = numeric_cast<int64_t>(*reinterpret_cast<int*>(data));
            break;
        case HLHDF_LONG:
            val = numeric_cast<int64_t>(*reinterpret_cast<int32_t*>(data));
            break;
        case HLHDF_LLONG:
            val = *reinterpret_cast<int64_t*>(data);
            break;
        default:
            throw brfc_error("invalid format");
    }
    return val;
}

HL_Data
IntConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::LongLong);
    
    int64_t v = value.toLongLong();
    return HL_Data(sizeof(int64_t), "llong",
                   reinterpret_cast<unsigned char*>(&v));
}

QVariant
StringConverter::do_convert(HL_FormatSpecifier format,
                            unsigned char* data) const {
    BRFC_ASSERT(format == HLHDF_STRING);
    return QVariant(QString(reinterpret_cast<char*>(data)));
}

HL_Data
StringConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::String);
    
    //XXX: test for Unicode strings!
    std::string v = value.toString().toStdString();
    return HL_Data(v.size() + 1, "string",
                   reinterpret_cast<unsigned char*>(
                         const_cast<char*>(v.c_str())));
}


QVariant
DateConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    QString str = StringConverter::do_convert(format, data).toString();
    QDate date =  QDate::fromString(str, "yyyyMMdd");
    BRFC_ASSERT(date.isValid());
    return date;
}

HL_Data
DateConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::Date);
    QString str = value.toDate().toString("yyyyMMdd");
    return StringConverter::do_convert(str);
}

QVariant
TimeConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    QString str = StringConverter::do_convert(format, data).toString();
    QTime time = QTime::fromString(str, "hhmmss");
    BRFC_ASSERT(time.isValid());
    return time;
}

HL_Data
TimeConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::Time);
    QString str = value.toTime().toString("hhmmss");
    return StringConverter::do_convert(str);
}

QVariant
BoolConverter::do_convert(HL_FormatSpecifier format,
                          unsigned char* data) const {
    QString str = StringConverter::do_convert(format, data).toString();
    bool val = false;
    if (str == "True") {
        val = true;
    } else if (str == "False") {
        val = false;
    } else {
        BRFC_ASSERT(false);
    }
    return val;
}

HL_Data
BoolConverter::do_convert(const QVariant& value) const {
    BRFC_ASSERT(value.type() == QVariant::Bool);

    QString bool_str;
    if (value.toBool()) {
        bool_str = "False";
    } else {
        bool_str = "True";
    }

    StringConverter str_conv;

    return str_conv.convert(bool_str);
}

} // namespace brfc
