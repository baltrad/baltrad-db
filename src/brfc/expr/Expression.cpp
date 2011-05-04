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

#include <brfc/expr/Expression.hpp>

#include <ostream>

#include <brfc/assert.hpp>

namespace brfc {
namespace expr {

Expression::Expression()
        : value_() {
}

Expression::Expression(bool value)
        : value_(value) {
}

Expression::Expression(int value)
        : value_((long long)value) {

}

Expression::Expression(long long value)
        : value_(value) {
}

Expression::Expression(double value)
        : value_(value) {
}

Expression::Expression(const char* value)
        : value_(std::string(value)) {
}

Expression::Expression(const std::string& value)
        : value_(value) {
}

Expression::Expression(const list_t& value)
        : value_(value) {
}

Expression::Expression(const Date& value)
        : value_(value) {
}

Expression::Expression(const Time& value)
        : value_(value) {
}

Expression::Expression(const DateTime& value)
        : value_(value) {
}

Expression::Expression(const TimeDelta& value)
        : value_(value) {
}

/*
Expression::Expression(const value_t& value)
        : value_(value) {

}
*/

namespace {
    static std::string null_prefix("\0", 1);
} // namespace anonymous

Expression::Expression(const std::string& value, construct_symbol)
        : value_(null_prefix + value) {
    // prefixing with \0 to mark symbols snatched from Joel de Guzman's utree
}

Expression
Expression::symbol(const std::string& value) {
    return Expression(value, construct_symbol());
}

Expression::Expression(const Expression& other)
        : value_(other.value_) {
}

void
Expression::swap(Expression& other) {
    std::swap(value_, other.value_);
}

Expression&
Expression::operator=(Expression rhs) {
    rhs.swap(*this);
    return *this;
}

Expression::operator Expression::bool_type() const {
    return (is_list() and empty()) ?
        0 : &Expression::this_type_does_not_support_comparisons;
}

namespace {

struct type_visitor : public boost::static_visitor<Expression::type::_> {
    Expression::type::_ operator()(const Expression::list_t&) const {
        return Expression::type::LIST;
    }

    Expression::type::_ operator()(bool) const {
        return Expression::type::BOOL;
    }

    Expression::type::_ operator()(long long) const {
        return Expression::type::INT64;
    }
    
    Expression::type::_ operator()(double) const {
        return Expression::type::DOUBLE;
    }

    Expression::type::_ operator()(const std::string& val) const {
        if (not val.empty() and val.at(0) == '\0')
            return Expression::type::SYMBOL;
        else
            return Expression::type::STRING;
    }

    Expression::type::_ operator()(const Date&) const {
        return Expression::type::DATE;
    }

    Expression::type::_ operator()(const Time&) const {
        return Expression::type::TIME;
    }

    Expression::type::_ operator()(const DateTime&) const {
        return Expression::type::DATETIME;
    }

    Expression::type::_ operator()(const TimeDelta&) const {
        return Expression::type::INTERVAL;
    }
};

} // namespace anonymous

Expression::type::_
Expression::type() const {
    return boost::apply_visitor(type_visitor(), value_);
}

Expression::list_t
Expression::list() const {
    return boost::get<list_t>(value_);
}

bool
Expression::bool_() const {
    return boost::get<bool>(value_);
}

long long
Expression::int64() const {
    return boost::get<long long>(value_);
}

double
Expression::double_() const {
    return boost::get<double>(value_);
}

std::string
Expression::string() const {
    return boost::get<std::string>(value_);
}

Date
Expression::date() const {
    return boost::get<Date>(value_);
}

Time
Expression::time() const {
    return boost::get<Time>(value_);
}

DateTime
Expression::datetime() const {
    return boost::get<DateTime>(value_);
}

std::string
Expression::symbol() const {
    BRFC_ASSERT(is_symbol());
    const std::string& s = boost::get<std::string>(value_);
    return s.substr(1);
}

size_t
Expression::size() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.size();
}

bool
Expression::empty() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.empty();
}

const Expression&
Expression::front() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.front();
}

void
Expression::pop_front() {
    BRFC_ASSERT(is_list());
    list_t& l = boost::get<list_t>(value_);
    BRFC_ASSERT(not l.empty());
    l.pop_front();
}

void
Expression::push_back(const Expression& value) {
    BRFC_ASSERT(is_list());
    list_t& l = boost::get<list_t>(value_);
    l.push_back(value);
}

Expression::const_iterator
Expression::begin() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.begin();
}

Expression::const_iterator
Expression::end() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.end();
}

namespace {

struct is_equal : public static_visitor<bool> {
    template<typename T, typename U>
    bool dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return lhs == rhs;
    }

    template<typename T, typename U>
    bool dispatch(const T& lhs, const U& rhs, boost::mpl::false_) const {
        return false;
    }

    template<typename T, typename U>
    bool operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    template<typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs == rhs;
    }
};

} // namespace anonymous

bool
operator==(const Expression& lhs, const Expression& rhs) {
    return apply_visitor(is_equal(), lhs, rhs);
}

namespace {

struct is_less_than : public static_visitor<bool> {
    template<typename T, typename U>
    bool dispatch(const T& lhs, const U& rhs, boost::mpl::true_) const {
        return lhs < rhs;
    }

    template<typename T, typename U>
    bool dispatch(const T& lhs, const U& rhs, boost::mpl::false_) const {
        return false;
    }

    template<typename T, typename U>
    bool operator()(const T& lhs, const U& rhs) const {
        return dispatch(lhs, rhs,
                        boost::mpl::and_<
                            boost::is_arithmetic<T>,
                            boost::is_arithmetic<U> >());
    }

    template<typename T>
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

} // namespace anonymous

bool
operator<(const Expression& lhs, const Expression& rhs) {
    return apply_visitor(is_less_than(), lhs, rhs);
}

namespace {

struct to_ostream : public static_visitor<void> {
    std::ostream& out;

    explicit to_ostream(std::ostream& out_) : out(out_) { }

    template<typename T>
    void operator()(const T& x) const {
        out << x;
    }

    void operator()(bool val) const {
        out << std::boolalpha << val;
    }

    void operator()(const std::string& str) const {
        if (not str.empty() and str.at(0) == '\0') {
            out << str.substr(1);
        } else {
            out << '"' << str << '"';
        }
    }

    void operator()(const Expression::list_t& x) const {
        out << "(";
        Expression::list_t::const_iterator it = x.begin();
        while (it != x.end()) {
            apply_visitor(*this, *it);
            ++it;
            if (it != x.end())
                out << " ";
        }
        out << ")";
    }

    void operator()(const Date& d) const {
        out << "(date \"" << d.to_iso_string() << "\")";
    }

    void operator()(const Time& t) const {
        out << "(time \"" << t.to_iso_string() << "\")";
    }

    void operator()(const DateTime& dt) const {
        out << "(datetime \"" << dt.to_iso_string() << "\")";
    }
};

} // namespace anonymous

std::ostream&
operator<<(std::ostream& out, const Expression& x) {
    apply_visitor(to_ostream(out), x);
    return out;
}

void PrintTo(const Expression& x, ::std::ostream* os) {
  *os << x;
}

} // namespace expr
} // namespace brfc
