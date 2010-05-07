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

#include <QtCore/QStringList>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

namespace brfc {

String::String()
        : value_("") {

}

String::String(const char* value)
        : value_(QString::fromUtf8(value)) {

}

String::String(const std::string& value)
        : value_(QString::fromUtf8(value.c_str())) {

}

String::String(const QString& value)
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

std::string
String::to_utf8() const {
    return std::string(value_.toUtf8().constData());
}

const String::uchar*
String::utf16() const {
    return value_.utf16();
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
              String::SplitPolicy split) const {
    StringList list;
    BOOST_FOREACH(const QString& qstr, value_.split(sep.value_)) {
        if (split == SKIP_EMPTY_PARTS and qstr.isEmpty())
            continue;
        list.append(qstr);
    }
    return list;
}

String&
String::append(const String& str) {
    value_.append(str.value_);
    return *this;
}

String&
String::prepend(const String& str) {
    value_.prepend(str.value_);
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
    value_.remove(str.value_);
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
        value_.replace(before.value_, after.value_);
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
    return value_.lastIndexOf(str.value_, from);
}

bool
String::contains(const String& str) const {
    return value_.contains(str.value_);
}

bool
String::starts_with(const String& str) const {
    if (str == "")
        return false;
    return value_.startsWith(str.value_);
}

String
String::right_justified(int width, String::uchar ch) const {
    return String(value_.rightJustified(width, ch));
}

String
String::section(const String& sep, int start, int end) const {
    return value_.section(sep.value_, start, end);
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
    return value_.at(0).unicode();
}

String
String::to_lower() const {
    return value_.toLower();
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

