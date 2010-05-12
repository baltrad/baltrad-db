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

#include <brfc/String.hpp>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <unicode/ucnv.h>

#include <brfc/exceptions.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/StringList.hpp>

namespace brfc {

String::String()
        : value_("") {

}

String::String(const char* value)
        : value_(value, "UTF-8") {

}

String::String(const std::string& value)
        : value_(value.c_str(), "UTF-8") {

}

String::String(const UnicodeString& value)
        : value_(value) {

}

String::String(const String& other)
        : value_(other.value_) {

}

String&
String::operator=(const String& rhs) {
    if (this != &rhs) {
        value_ = rhs.value_;
    }
    return *this;
}

namespace {

class UConv {
  public:
    UConv(const char* codepage)
            : err_(U_ZERO_ERROR)
            , conv_(0) {
        conv_ = ucnv_open(codepage, &err_);
        if (err_)
            throw std::runtime_error("could not create converter");
    }

    int max_bytes_for_string(const String& str) {
        return UCNV_GET_MAX_BYTES_FOR_STRING(str.length(),
                                             ucnv_getMaxCharSize(conv_));
    }

    std::string from_utf16(const String& str) {
        int max_bytes = max_bytes_for_string(str);
        auto_ptr<char> buf(new char[max_bytes]);
        ucnv_fromUChars(conv_,
                        buf.get(), max_bytes,
                        str.utf16(), str.length(),
                        &err_); 
        return std::string(buf.get());
    }

    ~UConv() {
        ucnv_close(conv_);
    }

  private:
    UErrorCode err_;
    UConverter* conv_;
};

}

std::string
String::to_utf8() const {
    return UConv("UTF-8").from_utf16(*this);
}

const String::uchar*
String::utf16() const {
    String* that = const_cast<String*>(this);
    return that->value_.getTerminatedBuffer();
}

String
String::from_utf16(const String::uchar* unicode, int length) {
    if (length == -1)
        return UnicodeString(unicode);
    else
        return UnicodeString(unicode, length);
}

String&
String::set_utf16(const String::uchar* unicode, int length) {
    value_.setTo(unicode, length);
    return *this;
}

namespace {

template<typename T>
String number_to_string(T num) {
    try {
        return String(boost::lexical_cast<std::string>(num));
    } catch (boost::bad_lexical_cast) {
        throw value_error("conversion failed");
    }
}

} // namespace anonymous

String
String::number(int num) {
    return number_to_string(num);
}

String
String::number(unsigned int num) {
    return number_to_string(num);
}

String
String::number(long long num) {
    return number_to_string(num);
}

String
String::number(double num) {
    return number_to_string(num);
}

int
String::length() const {
    return value_.length();
}

StringList
String::split(const String& sep,
              String::SplitPolicy policy) const {
    if (sep.length() == 0)
        throw value_error("empty separator");
    StringList list;
    int start = 0, end = 0;
    while ((end = index_of(sep, start)) != -1) {
        if (start != end or policy == KEEP_EMPTY_PARTS)
            list.append(substr(start, end - start));
        start = end + sep.length();
    }
    if (start != length())
        list.append(substr(start));
    else if (policy == KEEP_EMPTY_PARTS) {
        list.append("");
    }
    return list;
}

String&
String::append(const String& str) {
    value_.append(str.value_);
    return *this;
}

String&
String::append(String::uchar ch) {
    value_.append(ch);
    return *this;
}

String&
String::prepend(const String& str) {
    UnicodeString newstr(str.value_);
    newstr.append(value_);
    value_ = newstr;
    return *this;
}

String&
String::insert(int pos, const String& str) {
    if (pos < 0)
        throw value_error("insert position must be positive");
    if (pos > length())
        throw value_error("insert position outside of string");
    value_.insert(pos, str.value_);
    return *this;
}

String&
String::remove(const String& str) {
    value_.findAndReplace(str.value_, UnicodeString());
    return *this;
}

String&
String::remove(int pos, int n) {
    if (pos < 0)
        throw value_error("remove position must be positive");
    if (pos > length())
        throw value_error("remove position outside of string");
    value_.remove(pos, n);
    return *this;
}

String&
String::replace(const String& before, const String& after) {
    if (not before.value_.isEmpty())
        value_.findAndReplace(before.value_, after.value_);
    return *this;
}

String&
String::replace(int pos, int n, const String& after) {
    if (pos < 0)
        throw value_error("replace position must be positive");
    if (pos > length())
        throw value_error("replace position outside of string");
    value_.replace(pos, n, after.value_);
    return *this;
}

int
String::last_index_of(const String& str, int from) const {
    if (from < 0) {
        from += length();
    }
    return value_.lastIndexOf(str.value_, 0, from + str.length());
}

int
String::index_of(const String& str, int from) const {
    return value_.indexOf(str.value_, from);
}

bool
String::contains(const String& str) const {
    if (str.length() == 0)
        return true; // all strigns contain empty string
    return index_of(str, 0) >= 0;
}

bool
String::starts_with(const String& str) const {
    return value_.startsWith(str.value_);
}

String
String::right_justified(int width, String::uchar ch) const {
    String str(value_);
    str.value_.padLeading(width, ch);
    return str;
}

String
String::section(const String& sep, int start, int end) const {
    StringList strings = split(sep);

    if (start < 0)
        start += strings.size();
    if (end < 0)
        end += strings.size();

    StringList found;
    for (int i = start; i <= end; ++i) {
        found.append(strings.at(i));        
    }
    return found.join(sep);
}

int
String::to_int() const {
    try {
        return boost::lexical_cast<int>(to_std_string());
    } catch (boost::bad_lexical_cast&) {
        throw value_error("couldn't convert '" + to_utf8() + "' to int");
    }
}

String::uchar
String::char_at(int pos) const {
    if (pos < 0 or pos >= length())
        throw value_error("invalid string index");
    return value_.charAt(pos);
}

String
String::substr(int pos, int len) const {
    if (pos < 0 or pos >= length())
        throw value_error("invalid string index");
    if (len == -1)
        len = length();
    return UnicodeString(value_, pos, len);
}

String
String::to_lower() const {
    String str(value_);
    str.value_.toLower();
    return str;
}

bool
String::operator==(const String& rhs) const {
    return value_ == rhs.value_;
}

bool
String::operator<(const String& rhs) const {
    return value_ < rhs.value_;
}

bool
operator==(const char* s1, const String& s2) {
    return String(s1) == s2;
}

String
operator+(const char* s1, const String& s2) {
    return String(s1) + s2;
}

} // namespace brfc

std::ostream&
operator<<(std::ostream& out, const brfc::String& value) {
    out << value.to_utf8();
    return out;
}

