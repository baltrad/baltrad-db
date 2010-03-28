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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

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

const QString&
Variant::string() const {
    return get<const QString&>();
}

long long
Variant::longlong() const {
    return get<long long>();
}

double
Variant::double_() const {
    return get<double>();
}

bool
Variant::bool_() const {
    return get<bool>();
}

const QDate&
Variant::date() const {
    return get<const QDate&>();
}

const QTime&
Variant::time() const {
    return get<const QTime&>();
}

namespace {

/**
 * @brief visitor converting Variant to QVariant
 */
class variant_to_qvariant : public boost::static_visitor<QVariant> {
  public:
    template<typename T>
    QVariant operator()(const T& value) const {
        return QVariant(value);
    }
};

/**
 * @brief visitor converting Variant to QString
 */
class variant_to_string : public boost::static_visitor<QString> {
  public:
    QString operator()(const QString& value) const {
        return value;
    }

    QString operator()(long long value) const {
        return QString::number(value);
    }

    QString operator()(double value) const {
        return QString::number(value);
    }

    QString operator()(bool value) const {
        return value ? "True" : "False";
    }

    QString operator()(const QDate& value) const {
        return value.toString("yyyyMMdd");
    }

    QString operator()(const QTime& value) const {
        return value.toString("hhmmss");
    }
};

} // namespace anonymous

QVariant
Variant::to_qvariant() const {
    if (type_ == NONE)
        return QVariant();
    return boost::apply_visitor(variant_to_qvariant(), value_);
}

QString
Variant::to_string() const {
    return boost::apply_visitor(variant_to_string(), value_);
}

template<typename T>
T Variant::get() const {
    try {
        return boost::get<T>(value_);
    } catch (const boost::bad_get&) {
        throw value_error("held variant (" +
                          std::string(value_.type().name()) +
                          ") is not of requested type (" +
                          std::string(typeid(T).name()) + ")");
    }
}


bool
operator==(const Variant& lhs, const Variant& rhs) {
    return lhs.type_ == rhs.type_ and lhs.value_ == rhs.value_;
}

} // namespace brfc
