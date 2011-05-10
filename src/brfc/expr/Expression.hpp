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
#ifndef BRFC_EXPR_EXPRESSION_HPP
#define BRFC_EXPR_EXPRESSION_HPP

#include <iosfwd>
#include <list>

#include <boost/variant.hpp>

#include <brfc/DateTime.hpp>
#include <brfc/TimeDelta.hpp>

namespace brfc {
namespace expr {

class Expression {
    typedef void (Expression::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}

  public:
    struct construct_symbol {}; ///< construct symbol tag

    struct type {
        enum _ {
            LIST,
            BOOL,
            INT64,
            DOUBLE,
            STRING,
            DATE,
            TIME,
            DATETIME,
            INTERVAL,
            SYMBOL
        };
    };
    
    typedef std::list<Expression> list_t;

    typedef ::boost::variant<
                list_t,
                std::string,
                long long,
                double,
                bool,
                Date,
                Time,
                DateTime,
                TimeDelta
            > value_t;
    
    typedef list_t::const_iterator const_iterator;
    
    /**
     * @brief construct empty list
     */
    Expression();

    /**
     * @brief construct bool
     */
    explicit Expression(bool value);
    
    /**
     * @brief construct in64
     */
    explicit Expression(int value);

    /**
     * @brief construct int64
     */
    explicit Expression(long long value);

    /**
     * @brief construct double
     */
    explicit Expression(double value);
    
    /**
     * @brief construct string
     */
    explicit Expression(const char* value);

    /**
     * @brief construct string
     */
    explicit Expression(const std::string& value);

    /**
     * @brief construct symbol
     */
    explicit Expression(const std::string& value, construct_symbol);

    /**
     * @brief construct list
     */
    explicit Expression(const list_t& value);
    
    /**
     * @brief construct date
     */
    explicit Expression(const Date& value);
    
    /**
     * @brief construct time
     */
    explicit Expression(const Time& value);
    
    /**
     * @brief construct datetime
     */
    explicit Expression(const DateTime& value);
    
    /**
     * @brief construct interval
     */
    explicit Expression(const TimeDelta& value);
    
    /**
     * @brief construct list from iterators
     */
    template<typename Iter>
    Expression(Iter first, Iter last)
            : value_() {
        ::boost::get<list_t>(value_).assign(first, last);
    }
    
    /**
     * @brief construct symbol
     */
    static Expression symbol(const std::string& value);

    Expression(const Expression& other);
    Expression& operator=(Expression rhs);

    void swap(Expression& other);

    type::_ type() const;

    bool is_list() const { return type() == type::LIST; }
    bool is_bool() const { return type() == type::BOOL; }
    bool is_int64() const { return type() == type::INT64; }
    bool is_double() const { return type() == type::DOUBLE; }
    bool is_string() const { return type() == type::STRING; }
    bool is_symbol() const { return type() == type::SYMBOL; }
    bool is_date() const { return type() == type::DATE; }
    bool is_time() const { return type() == type::TIME; }
    bool is_datetime() const { return type() == type::DATETIME; }
    bool is_interval() const { return type() == type::INTERVAL; }
    
    list_t list() const;
    bool bool_() const;
    long long int64() const;
    double double_() const;
    std::string string() const;
    std::string symbol() const;
    Date date() const;
    Time time() const;
    DateTime datetime() const;
    TimeDelta interval() const;

    size_t size() const;
    bool empty() const;

    const Expression& front() const;
    void pop_front();
    void push_back(const Expression& value);
    void extend(const Expression& other);

    const_iterator begin() const;
    const_iterator end() const;

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(Visitor& v, const Expression& rhs) const {
        return ::boost::apply_visitor(v, value_, rhs.value_);
    }

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(const Visitor& v, const Expression& rhs) const {
        return ::boost::apply_visitor(v, value_, rhs.value_);
    }

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(Visitor& v) const {
        return ::boost::apply_visitor(v, value_);
    }

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(const Visitor& v) const {
        return ::boost::apply_visitor(v, value_);
    }

    operator bool_type() const;

  private:
    value_t value_;
};

template<typename Result>
struct static_visitor : public ::boost::static_visitor<Result> { };

template<typename Visitor>
typename Visitor::result_type
apply_visitor(Visitor& v, const Expression& lhs, const Expression& rhs) {
    return lhs.apply_visitor(v, rhs);
}

template<typename Visitor>
typename Visitor::result_type
apply_visitor(const Visitor& v, const Expression& lhs, const Expression& rhs) {
    return lhs.apply_visitor(v, rhs);
}

template<typename Visitor>
typename Visitor::result_type
apply_visitor(Visitor& v, const Expression& x) {
    return x.apply_visitor(v);
}

template<typename Visitor>
typename Visitor::result_type
apply_visitor(const Visitor& v, const Expression& x) {
    return x.apply_visitor(v);
}

bool operator==(const Expression& lhs, const Expression& rhs);

bool operator<(const Expression& lhs, const Expression& rhs);

inline bool operator!=(const Expression& lhs, const Expression& rhs) {
    return not (lhs == rhs);
}

inline bool operator>(const Expression& lhs, const Expression& rhs) {
    return rhs < lhs;
}

inline bool operator>=(const Expression& lhs, const Expression& rhs) {
    return not (lhs < rhs);
}

inline bool operator<=(const Expression& lhs, const Expression& rhs) {
    return not (rhs < lhs);
}

std::ostream& operator<<(std::ostream& out, const Expression& x);

// define so that gtest doesn't try to print Expression as a container
void PrintTo(const Expression& x, ::std::ostream* os);

/**
 * @brief insert @c sep between elements of [@c first, @c last)
 * @return the formed list
 */
Expression
join(Expression::const_iterator first,
     Expression::const_iterator last,
     const Expression& sep);

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_EXPRESSION_HPP
