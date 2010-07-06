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

#include <boost/foreach.hpp>

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
Literal::to_datetime(DateTime& dt, const String& str) const {
    if (not str.starts_with(str_))
        throw value_error("Literal token '" + str_.to_std_string() +
                          "' not encountered");
    return str_.length();
}

String
IntToken::to_string(const DateTime& dt) const {
    int val = get_value(dt);
    String str = String::number(val);
    if (zero_prefixed_)
        str = str.right_justified(max_length_, '0');
    return str;
}

int
IntToken::to_datetime(DateTime& dt, const String& str) const {
    // XXX: refactor me!
    if (str.length() == 0)
        throw value_error("string length == 0");
    bool has_sign = str.char_at(0) == '-';

    int val = -1, start = 0, nchar = std::min(max_length_, str.length());

    if (has_sign) {
        if (not signed_)
            throw value_error("sign for unsigned field");
        start += 1;
        nchar = std::min(nchar, str.length() - start);
    }
    if (zero_prefixed_) {
        val = str.substr(start, nchar).to_int();
        if (str.length() < start + max_length_)
            throw value_error("invalid zero prefixed integer: "
                              + str.to_std_string());
    } else {
        while (nchar > 0) {
            try {
                val = str.substr(start, nchar).to_int();
                break;
            } catch (const value_error&) {
                --nchar;
            }
        }
        if (nchar == 0)
            throw value_error("unable to parse integer from: "
                              + str.to_std_string());
    }
   
    if (has_sign)
        val *= -1;
    set_value(dt, val);
    return nchar + (has_sign ? 1 : 0);
}


} // namespace dttok

shared_ptr<dttok::Token>
DateTimeParser::token_from_string(const String& str) {
    if (str.starts_with("'") and str.ends_with("'")) {
        String lit = str.substr(1, str.length() - 2);
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

DateTimeParser::DateTimeParser(const String& format)
        : tokens_(tokenize(format)) {

}

DateTimeParser::DateTimeParser(const DateTimeParser::TokenVector& format)
        : tokens_(format) {

}

DateTimeParser::~DateTimeParser() {

}

DateTimeParser::TokenVector
DateTimeParser::tokenize(const String& format) {
    // yyyy/MM/dd'yy'hhmmqwesszzz
    String tokchars = "yMdhHmsz", tokstr;
    String::uchar pch = format.char_at(0);
    tokstr += pch;
    TokenVector tokens;
    bool explicit_literal = false;

    for (int i = 1; i < format.length(); ++i) {
        String::uchar ch = format.char_at(i);

        if (ch != pch and not explicit_literal
                      and (tokchars.contains(ch)
                           or tokchars.contains(pch))) {
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
DateTimeParser::from_string(const String& str) {
    DateTime dt;
    int pos = 0;
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        String substr = str.substr(pos, token->max_length());
        pos += token->to_datetime(dt, substr);
    }
    return dt;
}

String
DateTimeParser::to_string(const DateTime& dt) {
    String str;
    BOOST_FOREACH(shared_ptr<dttok::Token> token, tokens_) {
        str += token->to_string(dt);
    }
    return str;
}

Date
DateTimeParser::date_from_string(const String& str) {
    return from_string(str).date();
}

String
DateTimeParser::date_to_string(const Date& date) {
    DateTime dt(date);
    return to_string(dt);
}

Time
DateTimeParser::time_from_string(const String& str) {
    return from_string(str).time();
}

String
DateTimeParser::time_to_string(const Time& time) {
    DateTime dt(time);
    return to_string(dt);
}

}
