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

#include <sstream>

#include <brfc/Variant.hpp>

#include <boost/lexical_cast.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

bool Variant::is_null() const { return type_ == NONE; }
bool Variant::is_string() const { return type_ == STRING; }
bool Variant::is_int64() const { return type_ == INT64; }
bool Variant::is_double() const { return type_ == DOUBLE; }
bool Variant::is_bool() const { return type_ == BOOL; }
bool Variant::is_date() const { return type_ == DATE; }
bool Variant::is_time() const { return type_ == TIME; }
bool Variant::is_timedelta() const { return type_ == TIMEDELTA; }


const std::string&
Variant::string() const {
    return get<const std::string&>();
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

const DateTime&
Variant::datetime() const {
    return get<const DateTime&>();
}

const TimeDelta&
Variant::timedelta() const {
    return get<const TimeDelta&>();
}

namespace {

/**
 * @brief visitor converting Variant to std::string
 */
class variant_to_string : public boost::static_visitor<std::string> {
  public:
    std::string operator()(const std::string& value) const {
        return value;
    }

    std::string operator()(long long value) const {
        return boost::lexical_cast<std::string>(value);
    }

    std::string operator()(double value) const {
        return boost::lexical_cast<std::string>(value);
    }

    std::string operator()(bool value) const {
        return value ? "True" : "False";
    }

    std::string operator()(const Date& value) const {
        return value.to_string("yyyy-MM-dd");
    }

    std::string operator()(const Time& value) const {
        return value.to_string("hh:mm:ss");
    }

    std::string operator()(const DateTime& value) const {
        return value.to_string("yyyy-MM-dd hh:mm:ss");
    }

    std::string operator()(const TimeDelta& value) const {
        std::stringstream ss;
        ss << "P" << value.days() << "DT" << value.msecs() / 1000 << "S";
        return ss.str();
    }
};

class variant_to_int64 : public boost::static_visitor<long long> {
  public:
    long long operator()(const std::string& value) const {
        try {
            return boost::lexical_cast<long long>(value);
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
    double operator()(const std::string& value) const {
        try {
            return boost::lexical_cast<double>(value);
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
    bool operator()(const std::string& value) const {
        if (value == "false" or value == "0" or value == "")
            return false;
        else
            return true;
    }

    bool operator()(const Time&) const { return true; }
    bool operator()(const Date&) const { return true; }
    bool operator()(const DateTime&) const { return true; }
    bool operator()(const TimeDelta&) const { return true; }

    template<typename T>
    bool operator()(const T& value) const {
        return value;
    }
};

class variant_to_time : public boost::static_visitor<Time> {
  public:
    Time operator()(const std::string& value) const {
        return Time::from_string(value, "hh:mm:ss");
    }

    Time operator()(const Time& value) const { return value; }

    Time operator()(const DateTime& value) const { return value.time(); }

    template<typename T>
    Time operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to Time");
    }
};

class variant_to_date : public boost::static_visitor<Date> {
  public:
    Date operator()(const std::string& value) const {
        return Date::from_string(value, "yyyy-MM-dd");
    }

    Date operator()(const Date& value) const { return value; }

    Date operator()(const DateTime& value) const { return value.date(); }

    template<typename T>
    Date operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to Date");
    }
};

class variant_to_datetime : public boost::static_visitor<DateTime> {
  public:
    DateTime operator()(const std::string& value) const {
        return DateTime::from_string(value, "yyyy-MM-dd hh:mm:ss");
    }

    DateTime operator()(const DateTime& value) const { return value; }

    template<typename T>
    DateTime operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to DateTime");
    }
};

class variant_to_timedelta : public boost::static_visitor<TimeDelta> {
  public:
    TimeDelta operator()(const TimeDelta& value) const { return value; }

    template<typename T>
    TimeDelta operator()(const T&) const {
        throw value_error("held variant (" +
                          std::string(typeid(T).name()) + ") "
                          "can't be converted to TimeDelta");
    }
};


} // namespace anonymous

std::string
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

DateTime
Variant::to_datetime() const {
    if (type_ == NONE)
        throw value_error("held variant (NULL) can't be converted to DateTime");
    return boost::apply_visitor(variant_to_datetime(), value_);
}

TimeDelta
Variant::to_timedelta() const {
    if (type_ == NONE)
        throw value_error("held variant (NULL) can't be converted to TimeDelta");
    return boost::apply_visitor(variant_to_timedelta(), value_);
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
