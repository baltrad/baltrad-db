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
#ifndef BRFC_EXPR_SEXP_HPP
#define BRFC_EXPR_SEXP_HPP

#include <iosfwd>
#include <list>

#include <boost/variant.hpp>
#include <boost/ptr_container/ptr_list.hpp>

namespace brfc {
namespace expr {

class sexp {
  public:
    struct construct_symbol {}; ///< construct symbol tag

    struct type {
        enum _ {
            LIST,
            BOOL,
            INT64,
            DOUBLE,
            STRING,
            SYMBOL
        };
    };
    
    typedef std::list<sexp> list_t;

    typedef ::boost::variant<
                list_t,
                std::string,
                long long,
                double,
                bool
            > value_t;
    
    typedef list_t::const_iterator const_iterator;
    
    /**
     * @brief construct empty list
     */
    sexp();

    /**
     * @brief construct bool
     */
    sexp(bool value);

    sexp(int value);

    /**
     * @brief construct int64
     */
    sexp(long long value);

    /**
     * @brief construct double
     */
    sexp(double value);
    
    /**
     * @brief construct string
     */
    sexp(const char* value);

    /**
     * @brief construct string
     */
    sexp(const std::string& value);

    /**
     * @brief construct symbol
     */
    sexp(const std::string& value, construct_symbol);

    /**
     * @brief construct list
     */
    sexp(const list_t& value);
    
    /**
     * @brief construct list from iterators
     */
    template<typename Iter>
    sexp(Iter first, Iter last)
            : value_() {
        ::boost::get<list_t>(value_).assign(first, last);
    }
    
    /**
     * @brief construct symbol
     */
    static sexp symbol(const std::string& value);

    sexp(const sexp& other);
    sexp& operator=(sexp rhs);

    void swap(sexp& other);

    type::_ type() const;

    bool is_list() const { return type() == type::LIST; }
    bool is_bool() const { return type() == type::BOOL; }
    bool is_int64() const { return type() == type::INT64; }
    bool is_double() const { return type() == type::DOUBLE; }
    bool is_string() const { return type() == type::STRING; }
    bool is_symbol() const { return type() == type::SYMBOL; }
    
    list_t list() const;
    bool bool_() const;
    long long int64() const;
    double double_() const;
    std::string string() const;
    std::string symbol() const;

    size_t size() const;
    bool empty() const;

    const sexp& front() const;
    void pop_front();
    void push_back(const sexp& value);

    const_iterator begin() const;
    const_iterator end() const;

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(Visitor v, const sexp& rhs) const {
        return ::boost::apply_visitor(v, value_, rhs.value_);
    }

    template<typename Visitor>
    typename Visitor::result_type
    apply_visitor(Visitor v) const {
        return ::boost::apply_visitor(v, value_);
    }

  private:
    value_t value_;
};

template<typename Result>
struct static_visitor : public ::boost::static_visitor<Result> { };

template<typename Visitor>
typename Visitor::result_type
apply_visitor(Visitor v, const sexp& lhs, const sexp& rhs) {
    return lhs.apply_visitor(v, rhs);
}

template<typename Visitor>
typename Visitor::result_type
apply_visitor(Visitor v, const sexp& x) {
    return x.apply_visitor(v);
}

bool operator==(const sexp& lhs, const sexp& rhs);

bool operator<(const sexp& lhs, const sexp& rhs);

inline bool operator!=(const sexp& lhs, const sexp& rhs) {
    return not (lhs == rhs);
}

inline bool operator>(const sexp& lhs, const sexp& rhs) {
    return rhs < lhs;
}

inline bool operator>=(const sexp& lhs, const sexp& rhs) {
    return not (lhs < rhs);
}

inline bool operator<=(const sexp& lhs, const sexp& rhs) {
    return not (rhs < lhs);
}

std::ostream& operator<<(std::ostream& out, const sexp& x);

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_SEXP_HPP
