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

#include <brfc/expr/sexp.hpp>

#include <ostream>

#include <brfc/assert.hpp>

namespace brfc {
namespace expr {

sexp::sexp()
        : value_() {
}

sexp::sexp(bool value)
        : value_(value) {
}

sexp::sexp(int value)
        : value_((long long)value) {

}

sexp::sexp(long long value)
        : value_(value) {
}

sexp::sexp(double value)
        : value_(value) {
}

sexp::sexp(const char* value)
        : value_(std::string(value)) {
}

sexp::sexp(const std::string& value)
        : value_(value) {
}

sexp::sexp(const list_t& value)
        : value_(value) {
}

sexp::sexp(const Date& value)
        : value_(value) {
}

sexp::sexp(const Time& value)
        : value_(value) {
}

sexp::sexp(const DateTime& value)
        : value_(value) {
}

/*
sexp::sexp(const value_t& value)
        : value_(value) {

}
*/

namespace {
    static std::string null_prefix("\0", 1);
} // namespace anonymous

sexp::sexp(const std::string& value, construct_symbol)
        : value_(null_prefix + value) {
    // prefixing with \0 to mark symbols snatched from Joel de Guzman's utree
}

sexp
sexp::symbol(const std::string& value) {
    return sexp(value, construct_symbol());
}

sexp::sexp(const sexp& other)
        : value_(other.value_) {
}

void
sexp::swap(sexp& other) {
    std::swap(value_, other.value_);
}

sexp&
sexp::operator=(sexp rhs) {
    rhs.swap(*this);
    return *this;
}

namespace {

struct type_visitor : public boost::static_visitor<sexp::type::_> {
    sexp::type::_ operator()(const sexp::list_t&) const {
        return sexp::type::LIST;
    }

    sexp::type::_ operator()(bool) const {
        return sexp::type::BOOL;
    }

    sexp::type::_ operator()(long long) const {
        return sexp::type::INT64;
    }
    
    sexp::type::_ operator()(double) const {
        return sexp::type::DOUBLE;
    }

    sexp::type::_ operator()(const std::string& val) const {
        if (not val.empty() and val.at(0) == '\0')
            return sexp::type::SYMBOL;
        else
            return sexp::type::STRING;
    }

    sexp::type::_ operator()(const Date&) const {
        return sexp::type::DATE;
    }

    sexp::type::_ operator()(const Time&) const {
        return sexp::type::TIME;
    }

    sexp::type::_ operator()(const DateTime&) const {
        return sexp::type::DATETIME;
    }
};

} // namespace anonymous

sexp::type::_
sexp::type() const {
    return boost::apply_visitor(type_visitor(), value_);
}

sexp::list_t
sexp::list() const {
    return boost::get<list_t>(value_);
}

bool
sexp::bool_() const {
    return boost::get<bool>(value_);
}

long long
sexp::int64() const {
    return boost::get<long long>(value_);
}

double
sexp::double_() const {
    return boost::get<double>(value_);
}

std::string
sexp::string() const {
    return boost::get<std::string>(value_);
}

Date
sexp::date() const {
    return boost::get<Date>(value_);
}

Time
sexp::time() const {
    return boost::get<Time>(value_);
}

DateTime
sexp::datetime() const {
    return boost::get<DateTime>(value_);
}

std::string
sexp::symbol() const {
    BRFC_ASSERT(is_symbol());
    const std::string& s = boost::get<std::string>(value_);
    return s.substr(1);
}

size_t
sexp::size() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.size();
}

bool
sexp::empty() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.empty();
}

const sexp&
sexp::front() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.front();
}

void
sexp::pop_front() {
    BRFC_ASSERT(is_list());
    list_t& l = boost::get<list_t>(value_);
    BRFC_ASSERT(not l.empty());
    l.pop_front();
}

void
sexp::push_back(const sexp& value) {
    BRFC_ASSERT(is_list());
    list_t& l = boost::get<list_t>(value_);
    l.push_back(value);
}

sexp::const_iterator
sexp::begin() const {
    BRFC_ASSERT(is_list());
    const list_t& l = boost::get<list_t>(value_);
    return l.begin();
}

sexp::const_iterator
sexp::end() const {
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
operator==(const sexp& lhs, const sexp& rhs) {
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
operator<(const sexp& lhs, const sexp& rhs) {
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

    void operator()(const sexp::list_t& x) const {
        out << "(";
        sexp::list_t::const_iterator it = x.begin();
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
operator<<(std::ostream& out, const sexp& x) {
    apply_visitor(to_ostream(out), x);
    return out;
}

} // namespace expr
} // namespace brfc
