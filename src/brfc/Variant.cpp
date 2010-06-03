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

bool Variant::is_null() const { return type_ == NONE; }
bool Variant::is_string() const { return type_ == STRING; }
bool Variant::is_long() const { return is_int64(); }
bool Variant::is_int64() const { return type_ == INT64; }
bool Variant::is_double() const { return type_ == DOUBLE; }
bool Variant::is_bool() const { return type_ == BOOL; }
bool Variant::is_date() const { return type_ == DATE; }
bool Variant::is_time() const { return type_ == TIME; }


const String&
Variant::string() const {
    return get<const String&>();
}

long long
Variant::longlong() const {
    return int64_();
    return get<long long>();
}

long long
Variant::int64_() const {
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

const Date&
Variant::date() const {
    return get<const Date&>();
}

const Time&
Variant::time() const {
    return get<const Time&>();
}

namespace {

/**
 * @brief visitor converting Variant to String
 */
class variant_to_string : public boost::static_visitor<String> {
  public:
    String operator()(const String& value) const {
        return value;
    }

    String operator()(long long value) const {
        return String::number(value);
    }

    String operator()(double value) const {
        return String::number(value);
    }

    String operator()(bool value) const {
        return value ? "True" : "False";
    }

    String operator()(const Date& value) const {
        return value.to_string("yyyyMMdd");
    }

    String operator()(const Time& value) const {
        return value.to_string("hhmmss");
    }
};

} // namespace anonymous

String
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
