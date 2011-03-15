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

#include <brfc/util/iso8601.hpp>

#include <iterator>

#include <boost/lexical_cast.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>

namespace ph = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ka = boost::spirit::karma;

BOOST_FUSION_ADAPT_STRUCT(
    brfc::iso8601::date,
    (int, year)
    (unsigned int, month)
    (unsigned int, day)
)

BOOST_FUSION_ADAPT_STRUCT(
    brfc::iso8601::time,
    (unsigned int, hour)
    (unsigned int, minute)
    (unsigned int, second)
    (unsigned int, usec)
)

BOOST_FUSION_ADAPT_STRUCT(
    brfc::iso8601::datetime,
    (brfc::iso8601::date, date)
    (brfc::iso8601::time, time)
)

namespace brfc {
namespace iso8601 {

template<typename Iterator>
struct date_parser : qi::grammar<Iterator, date()> {
    date_parser()
            : date_parser::base_type(start) {
        typedef qi::uint_parser<unsigned int, 10, 2, 2> uint2_p;

        year %= qi::int_parser<int, 10, 4, 4>();
        month %= uint2_p() [qi::_pass = qi::_1 >= 1u and qi::_1 <= 12u];
        day %= uint2_p() [qi::_pass = qi::_1 >= 1u and qi::_1 <= 31u];

        basic_date %= year >> month >> day;
        extended_date %= year >> "-" >> month >> "-" >> day;

        start %= basic_date | extended_date;
    }
    
    qi::rule<Iterator, date()> start;
    qi::rule<Iterator, date()> basic_date;
    qi::rule<Iterator, date()> extended_date;
    qi::rule<Iterator, int()> year;
    qi::rule<Iterator, unsigned int()> month;
    qi::rule<Iterator, unsigned int()> day;
};

struct fraction_impl {
    template <typename Arg1, typename Arg2>
    struct result {
        typedef unsigned int type;
    };

    template <typename Arg1, typename Arg2>
    unsigned int operator()(Arg1 zero_count, Arg2 n) const {
        size_t digit_count = count_digits(n);
        size_t total_count = zero_count + digit_count;
        if (total_count < 6) {
            return n * pow(10, 6 - zero_count - digit_count);
        } else if (total_count > 6) {
            return n / pow(10, (total_count - 6));
        } else {
            return n;
        }
    }

    unsigned int pow(unsigned int n, unsigned int p) const {
   		return boost::numeric_cast<unsigned int>(std::pow((float)n, (int)p));
	}

    template <class T>
    size_t count_digits(T n) const {
        size_t d = 0;
        if (n <= 0) d = 1;
        while (n) {
            n /= 10;
            ++d;
        }
        return d;
    }
};

template<typename Iterator>
struct usec_parser : qi::grammar<Iterator,
                            unsigned int(),
                            qi::locals<unsigned int> > {
    usec_parser()
            : usec_parser::base_type(start) {
        start %= *qi::lit('0') [++qi::_a]
              >> qi::uint_parser<unsigned int>() [qi::_val = fraction(qi::_a, qi::_1)]
        ;
    }

    qi::rule<Iterator, unsigned int(), qi::locals<unsigned int> > start;
    ph::function<fraction_impl> fraction;
};

template<typename Iterator>
struct time_parser : qi::grammar<Iterator, time()> {
    time_parser()
            : time_parser::base_type(start) {
        typedef qi::uint_parser<unsigned int, 10, 2, 2> uint2_p;

        hour %= uint2_p() [qi::_pass = qi::_1 <= 24u];
        minute %= uint2_p() [qi::_pass = qi::_1 <= 59u];
        second %= uint2_p() [qi::_pass = qi::_1 <= 60u];
        usec %= (qi::lit('.') | ',') >> usec_p;

        basic_time %= hour >> minute >> second >> -usec;
        extended_time %= hour >> ':' >> minute >> ':' >> second >> -usec;

        start %= basic_time | extended_time;
    }

    qi::rule<Iterator, time()> start;
    qi::rule<Iterator, time()> basic_time;
    qi::rule<Iterator, time()> extended_time;

    qi::rule<Iterator, unsigned int()> hour;
    qi::rule<Iterator, unsigned int()> minute;
    qi::rule<Iterator, unsigned int()> second;
    qi::rule<Iterator, unsigned int()> usec;
    usec_parser<Iterator> usec_p;
};

template<typename Iterator>
struct datetime_parser : qi::grammar<Iterator, datetime()> {
    datetime_parser()
            : datetime_parser::base_type(start) {
        // note that space is not actually a valid separator by iso8601, but
        // we accept it
        start %= date_p >> (qi::lit('T') | ' ') >> time_p;
    }

    qi::rule<Iterator, datetime()> start;
    date_parser<Iterator> date_p;
    time_parser<Iterator> time_p;
};

template<typename OutputIterator>
struct date_generator : ka::grammar<OutputIterator, date()> {
    date_generator(format::type fmt)
            : date_generator::base_type(start) {
        year = ka::right_align(4, '0')[ka::int_];
        month = ka::right_align(2, '0')[ka::uint_];
        day = ka::right_align(2, '0')[ka::uint_];

        if (fmt == format::EXTENDED) {
            start = year << '-' << month << '-' << day;
        } else {
            start = year << month << day;
        }
    }

    ka::rule<OutputIterator, date()> start;
    ka::rule<OutputIterator, int()> year;
    ka::rule<OutputIterator, unsigned int()> month;
    ka::rule<OutputIterator, unsigned int()> day;
};

template<typename OutputIterator>
struct time_generator : ka::grammar<OutputIterator, time()> {
    time_generator(format::type fmt, bool with_usec=true)
            : time_generator::base_type(start) {
        hour = ka::right_align(2, '0')[ka::uint_];
        minute = ka::right_align(2, '0')[ka::uint_];
        second = ka::right_align(2, '0')[ka::uint_];
        usec = '.' << ka::right_align(6, '0')[ka::uint_];
        if (not with_usec)
            usec = ka::omit[usec];

        if (fmt == format::EXTENDED) {
            start = hour << ':' << minute << ':' << second << usec;
        } else {
            start = hour << minute << second << usec;
        }
    }
    
    ka::rule<OutputIterator, time()> start;
    ka::rule<OutputIterator, unsigned int()> hour;
    ka::rule<OutputIterator, unsigned int()> minute;
    ka::rule<OutputIterator, unsigned int()> second;
    ka::rule<OutputIterator, unsigned int()> usec;
};

template<typename OutputIterator>
struct datetime_generator : ka::grammar<OutputIterator, datetime()> {
    datetime_generator(format::type fmt, bool with_usec=true)
            : datetime_generator::base_type(start)
            , date(fmt) 
            , time(fmt, with_usec) {
        start = date << 'T' << time;
    }
    
    ka::rule<OutputIterator, datetime()> start;
    date_generator<OutputIterator> date;
    time_generator<OutputIterator> time;
};

bool
parse_date(const std::string& str, date& d) {
    date_parser<std::string::const_iterator> p;
    std::string::const_iterator begin = str.begin();
    std::string::const_iterator end = str.end();
    return qi::parse(begin, end, p, d) and begin == end;
}

bool
parse_time(const std::string& str, time& t) {
    time_parser<std::string::const_iterator> p;
    std::string::const_iterator begin = str.begin();
    std::string::const_iterator end = str.end();
    return qi::parse(begin, end, p, t) and begin == end;
}

bool
parse_datetime(const std::string& str, datetime& dt) {
    datetime_parser<std::string::const_iterator> p;
    std::string::const_iterator begin = str.begin();
    std::string::const_iterator end = str.end();
    return qi::parse(begin, end, p, dt) and begin == end;
}

std::string
to_string(const date& d, format::type fmt) {
    std::string str;
    std::back_insert_iterator<std::string> sink(str);
    date_generator<std::back_insert_iterator<std::string> > gen(fmt);
    ka::generate(sink, gen, d);
    return str;
}

std::string
to_string(const time& t, format::type fmt) {
    std::string str;
    std::back_insert_iterator<std::string> sink(str);
    time_generator<std::back_insert_iterator<std::string> > gen(fmt, t.usec != 0);
    ka::generate(sink, gen, t);
    return str;
}

std::string
to_string(const datetime& dt, format::type fmt) {
    std::string str;
    std::back_insert_iterator<std::string> sink(str);
    datetime_generator<std::back_insert_iterator<std::string> > gen(fmt, dt.time.usec != 0);
    ka::generate(sink, gen, dt);
    return str;
}

} // namespace iso8601
} // namespace brfc
