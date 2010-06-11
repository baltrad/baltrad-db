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

#include <boost/lexical_cast.hpp>

#include <brfc/exceptions.hpp>

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
        return value.to_string("yyyy-MM-dd");
    }

    String operator()(const Time& value) const {
        return value.to_string("hh:mm:ss");
    }
};

class variant_to_int64 : public boost::static_visitor<long long> {
  public:
    long long operator()(const String& value) const {
        try {
            return boost::lexical_cast<long long>(value.to_std_string());
        } catch (const boost::bad_lexical_cast& e) {
            throw value_error(e.what());
        }
    }

    long long operator()(bool value) const { return value; }
    long long operator()(double value) const { return value; }
    long long operator()(long long value) const { return value; }

    template<typename T>
    long long operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to int64");
    }
};

class variant_to_double : public boost::static_visitor<double> {
  public:
    double operator()(const String& value) const {
        try {
            return boost::lexical_cast<double>(value.to_std_string());
        } catch (const boost::bad_lexical_cast& e) {
            throw value_error(e.what());
        }
    }

    double operator()(bool value) const { return value; }
    double operator()(double value) const { return value; }
    double operator()(long long value) const { return value; }

    template<typename T>
    double operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to double");
    }
};

class variant_to_bool : public boost::static_visitor<bool> {
  public:
    bool operator()(const String& value) const { return value != ""; }
    bool operator()(const Time&) const { return true; }
    bool operator()(const Date&) const { return true; }

    template<typename T>
    bool operator()(const T& value) const {
        return value;
    }
};

class variant_to_time : public boost::static_visitor<Time> {
  public:
    Time operator()(const String& value) const {
        return Time::from_string(value, "hh:mm:ss");
    }

    Time operator()(const Time& value) const { return value; }

    template<typename T>
    Time operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to Time");
    }
};

class variant_to_date : public boost::static_visitor<Date> {
  public:
    Date operator()(const String& value) const {
        return Date::from_string(value, "yyyy-MM-dd");
    }

    Date operator()(const Date& value) const { return value; }

    template<typename T>
    Date operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to Date");
    }
};

} // namespace anonymous

String
Variant::to_string() const {
    return boost::apply_visitor(variant_to_string(), value_);
}

long long
Variant::to_int64() const {
    if (type_ == NONE)
        return 0;
    return boost::apply_visitor(variant_to_int64(), value_);
}

double
Variant::to_double() const {
    if (type_ == NONE)
        return 0.0;
    return boost::apply_visitor(variant_to_double(), value_);
}

bool
Variant::to_bool() const {
    return boost::apply_visitor(variant_to_bool(), value_);
}

Time
Variant::to_time() const {
    if (type_ == NONE)
        throw value_error("held variant (NULL) can't be converted to Time");
    return boost::apply_visitor(variant_to_time(), value_);
}

Date
Variant::to_date() const {
    if (type_ == NONE)
        throw value_error("held variant (NULL) can't be converted to Date");
    return boost::apply_visitor(variant_to_date(), value_);
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
