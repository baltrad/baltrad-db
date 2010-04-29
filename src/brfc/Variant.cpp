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

#include <brfc/Variant.hpp>

#include <utility>

namespace brfc {

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
