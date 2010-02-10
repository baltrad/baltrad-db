#include <brfc/Variant.hpp>

#include <QtCore/QVariant>

#include <utility>

namespace brfc {

namespace {


} // namespace anonymous

Variant::Variant(const QVariant& value)
        : type_(NONE)
        , value_() {
    switch (value.type()) {
        case QVariant::Invalid:
            break;
        case QVariant::String:
            type_ = STRING;
            value_ = value.toString();
            break;
        case QVariant::Int:
        case QVariant::LongLong:
            type_ = LONGLONG;
            value_ = value.toLongLong();
            break;
        case QVariant::Double:
            type_ = DOUBLE;
            value_ = value.toDouble();
            break;
        case QVariant::Bool:
            type_ = BOOL;
            value_ = value.toBool();
            break;
        case QVariant::Date:
            type_ = DATE;
            value_ = value.toDate();
            break;
        case QVariant::Time:
            type_ = TIME;
            value_ = value.toTime();
            break;
        default:
            throw value_error("invalid value type held in QVariant");
    }
}


QVariant
Variant::to_qvariant() const {
    if (type_ == NONE)
        return QVariant();
    return boost::apply_visitor(variant_to_qvariant(), value_);
}

bool
operator==(const Variant& lhs, const Variant& rhs) {
    return lhs.type_ == rhs.type_ and lhs.value_ == rhs.value_;
}

} // namespace brfc
