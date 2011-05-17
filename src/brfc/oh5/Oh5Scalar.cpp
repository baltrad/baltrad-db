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

#include <brfc/oh5/Oh5Scalar.hpp>

#include <boost/lexical_cast.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

namespace brfc {

Oh5Scalar::Oh5Scalar(bool value)
        : value_(value ? "True" : "False") {

}

Oh5Scalar::Oh5Scalar(const Date& value)
        : value_(value.to_iso_string()) {

}

Oh5Scalar::Oh5Scalar(Time value)
        : value_() {
     value.msec(0);
     value_ = value.to_iso_string();
}

template<typename T>
T
Oh5Scalar::get() const {
    try {
        return boost::get<T>(value_);
    } catch (const boost::bad_get&) {
        throw value_error("held value (" +
                          std::string(value_.type().name()) +
                          ") is not of requested type (" +
                          std::string(typeid(T).name()) + ")");
    }
}

namespace {

class scalar_type : public boost::static_visitor<Oh5Scalar::Type> {
  public:
    Oh5Scalar::Type operator()(const std::string& value) const { return Oh5Scalar::STRING; }
    Oh5Scalar::Type operator()(long long value) const { return Oh5Scalar::INT64; }
    Oh5Scalar::Type operator()(double value) const {return Oh5Scalar::DOUBLE; }
};

} // namespace anonymous

Oh5Scalar::Type
Oh5Scalar::type() const {
    return boost::apply_visitor(scalar_type(), value_);
}

const std::string&
Oh5Scalar::string() const {
    return get<const std::string&>();
}

long long
Oh5Scalar::int64_() const {
    return get<long long>();
}

double
Oh5Scalar::double_() const {
    return get<double>();
}

namespace {

class scalar_to_string : public boost::static_visitor<std::string> {
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
};

class scalar_to_int64 : public boost::static_visitor<long long> {
  public:
    long long operator()(const std::string& value) const {
        try {
            return boost::lexical_cast<long long>(value);
        } catch (const boost::bad_lexical_cast& e) {
            throw value_error(e.what());
        }
    }

    long long operator()(long long value) const { return value; }
    long long operator()(double value) const { return value; }
};

class scalar_to_double : public boost::static_visitor<double> {
  public:
    double operator()(const std::string& value) const {
        try {
            return boost::lexical_cast<double>(value);
        } catch (const boost::bad_lexical_cast& e) {
            throw value_error(e.what());
        }
    }

    double operator()(long long value) const { return value; }
    double operator()(double value) const { return value; }
};

class scalar_to_bool : public boost::static_visitor<bool> {
  public:
    bool operator()(const std::string& value) const {
        if (value == "False")
            return false;
        else if (value == "True")
            return true;
        else
            throw value_error("can't convert '" + value + "' to bool");
    }

    bool operator()(long long value) const { return value; }
    bool operator()(double value) const { return value; }
};

class scalar_to_date : public boost::static_visitor<Date> {
  public:
    Date operator()(const std::string& value) const {
        if (value.length() != 8)
            throw value_error("Scalar not convertible to Date");
        return Date::from_iso_string(value);
    }

    template<typename T>
    Date operator()(const T&) const {
        throw value_error("can't convert type " +
                          std::string(typeid(T).name()) +
                          " to Date");
    }
};

class scalar_to_time : public boost::static_visitor<Time> {
  public:
    Time operator()(const std::string& value) const {
        if (value.length() != 6)
            throw value_error("Scalar not convertible to Time");
        return Time::from_iso_string(value);
    }

    template<typename T>
    Time operator()(const T&) const {
        throw value_error("can't convert type " +
                          std::string(typeid(T).name()) +
                          " to Time");
    }
};

} // namespace anonymous

std::string
Oh5Scalar::to_string() const {
    return boost::apply_visitor(scalar_to_string(), value_);
}

long long
Oh5Scalar::to_int64() const {
    return boost::apply_visitor(scalar_to_int64(), value_);
}

double
Oh5Scalar::to_double() const {
    return boost::apply_visitor(scalar_to_double(), value_);
}

bool
Oh5Scalar::to_bool() const {
    return boost::apply_visitor(scalar_to_bool(), value_);
}

Date
Oh5Scalar::to_date() const {
    return boost::apply_visitor(scalar_to_date(), value_);
}

Time
Oh5Scalar::to_time() const {
    return boost::apply_visitor(scalar_to_time(), value_);
}

bool
operator==(const Oh5Scalar& lhs, const Oh5Scalar& rhs) {
    return lhs.value_ == rhs.value_;
}

} // namespace brfc
