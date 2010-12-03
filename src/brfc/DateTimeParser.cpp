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

#include <brfc/DateTimeParser.hpp>

#include <algorithm>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

namespace dttok {

int
IntToken::max_length() const {
    int len = max_length_;
    if (signed_)
        len += 1;
    return len;
}

int
Literal::to_datetime(DateTime& dt, const std::string& str) const {
    if (not boost::starts_with(str, str_))
        throw value_error("Literal token '" + str_ +
                          "' not encountered");
    return str_.length();
}

std::string
IntToken::to_string(const DateTime& dt) const {
    std::stringstream ss;
    ss.fill('0');
    ss.width(max_length_);
    ss << get_value(dt);
    return ss.str();
}

int
IntToken::to_datetime(DateTime& dt, const std::string& str) const {
    // XXX: refactor me!
    if (str.length() == 0)
        throw value_error("string length == 0");
    bool has_sign = str.at(0) == '-';

    int val = -1;
    unsigned int start = 0;
    unsigned int nchar = std::min((size_t)max_length_, str.length());

    if (has_sign) {
        if (not signed_)
            throw value_error("sign for unsigned field");
        start += 1;
        nchar = std::min(nchar, str.length() - start);
    }
    if (zero_prefixed_) {
        try {
            val = boost::lexical_cast<int>(str.substr(start, nchar));
        } catch (const boost::bad_lexical_cast&) {
            throw value_error("could not cast '" + str.substr(start, nchar) +
                              "' to int");
        }
        if (str.length() < start + max_length_)
            throw value_error("invalid zero prefixed integer: " + str);
    } else {
        while (nchar > 0) {
            try {
                val = boost::lexical_cast<int>(str.substr(start, nchar));
                break;
            } catch (const boost::bad_lexical_cast&) {
                --nchar;
            }
        }
        if (nchar == 0)
            throw value_error("unable to parse integer from: " + str);
    }
   
    if (has_sign)
        val *= -1;
    set_value(dt, val);
    return nchar + (has_sign ? 1 : 0);
}


} // namespace dttok

shared_ptr<dttok::Token>
DateTimeParser::token_from_string(const std::string& str) {
    if (boost::starts_with(str, "'") and boost::ends_with(str, "'")) {
        std::string lit = str.substr(1, str.length() - 2);
        if (lit.length() == 0)
            lit = "'";
        return make_shared<dttok::Literal>(lit);
    } else if (str == "yyyy")
        return make_shared<dttok::Year>();
    else if (str == "yy")
        return make_shared<dttok::ShortYear>();
    else if (str == "MM")
        return make_shared<dttok::Month>(true);
    else if (str == "M")
        return make_shared<dttok::Month>(false);
    else if (str == "dd")
        return make_shared<dttok::Day>(true);
    else if (str == "d")
        return make_shared<dttok::Day>(false);
    else if (str == "hh" or str == "HH")
        return make_shared<dttok::Hour>(true);
    else if (str == "h" or str == "M")
        return make_shared<dttok::Hour>(false);
    else if (str == "mm")
        return make_shared<dttok::Minute>(true);
    else if (str == "m")
        return make_shared<dttok::Minute>(false);
    else if (str == "ss")
        return make_shared<dttok::Second>(true);
    else if (str == "s")
        return make_shared<dttok::Second>(false);
    else if (str == "zzz")
        return make_shared<dttok::Msec>(true);
    else if (str == "z")
        return make_shared<dttok::Msec>(false);
    else
        return make_shared<dttok::Literal>(str);
}

DateTimeParser::DateTimeParser(const std::string& format)
        : tokens_(tokenize(format)) {

}

DateTimeParser::DateTimeParser(const DateTimeParser::TokenVector& format)
        : tokens_(format) {

}

DateTimeParser::~DateTimeParser() {

}

DateTimeParser::TokenVector
DateTimeParser::tokenize(const std::string& format) {
    // yyyy/MM/dd'yy'hhmmqwesszzz
    std::string tokchars = "yMdhHmsz", tokstr;
    char pch = format.at(0);
    tokstr += pch;
    TokenVector tokens;
    bool explicit_literal = false;

    for (unsigned int i = 1; i < format.length(); ++i) {
        char ch = format.at(i);

        if (ch != pch and not explicit_literal
                      and (tokchars.find(ch) != std::string::npos
                           or tokchars.find(pch) != std::string::npos)) {
            tokens.push_back(token_from_string(tokstr));
            tokstr = "";
        }

        if (ch == '\'')
            explicit_literal = not explicit_literal;

        tokstr += ch;
        pch = ch;
    }
    if (tokstr.length() > 0)
        tokens.push_back(token_from_string(tokstr));
    return tokens;
}

bool
DateTimeParser::is_format_date_only() const {
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        if (dynamic_pointer_cast<dttok::TimeToken>(token))
            return false;
    }
    return true;
}

bool
DateTimeParser::is_format_time_only() const {
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        if (dynamic_pointer_cast<dttok::DateToken>(token))
            return false;
    }
    return true;
}

DateTime
DateTimeParser::from_string(const std::string& str) {
    DateTime dt;
    int pos = 0;
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        std::string substr = str.substr(pos, token->max_length());
        pos += token->to_datetime(dt, substr);
    }
    return dt;
}

std::string
DateTimeParser::to_string(const DateTime& dt) {
    std::string str;
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        str += token->to_string(dt);
    }
    return str;
}

Date
DateTimeParser::date_from_string(const std::string& str) {
    return from_string(str).date();
}

std::string
DateTimeParser::date_to_string(const Date& date) {
    DateTime dt(date);
    return to_string(dt);
}

Time
DateTimeParser::time_from_string(const std::string& str) {
    return from_string(str).time();
}

std::string
DateTimeParser::time_to_string(const Time& time) {
    DateTime dt(time);
    return to_string(dt);
}

}
