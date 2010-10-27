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

#ifndef BRFC_STRING_HPP
#define BRFC_STRING_HPP

#include <string>

#include <iosfwd>

#include <unicode/unistr.h>

namespace brfc {

class StringList;

class String {
  public:
    /**
     * @brief UTF-16 code unit
     */
    typedef unsigned short uchar;
    
    enum SplitPolicy {
        KEEP_EMPTY_PARTS,
        SKIP_EMPTY_PARTS
    };

    /**
     * @brief construct empty string
     */
    String();

    /**
     * @brief construct from UTF-8 char array
     */
    String(const char* value);

    /**
     * @brief construct from UTF-8 std::string
     */
    String(const std::string& value);

    String(const UnicodeString& value);

    /**
     * @brief copy constructor
     */
    String(const String& other);

    /**
     * @brief copy assignment
     */
    String& operator=(const String& rhs);
    
    /**
     * @brief convert to UTF-8 std::string
     * @sa to_utf8
     */
    std::string to_std_string() const {
        return to_utf8();
    }

    /**
     * @brief UTF-16 array
     * @return '\0'-terminated array of unsigned shorts
     * @sa utf16()
     */
    const uchar* unicode() const {
        return utf16();
    }
    
    /**
     * @brief set from UTF-16 array
     */
    String& set_utf16(const uchar* unicode, int length);

    /**
     * @brief construct from UTF-16 array
     * @param unicode array of UTF-16 code units
     * @param length number of unicode characters to copy.
     *        If -1 (the default), @a unicode must be NULL-terminated
     */
    static String from_utf16(const uchar* unicode, int length=-1);

    /**
     * @brief access UTF-16 code units
     * @return NULL-terminated array of UTF-16 code units
     *
     * @note the result remains valid until the string is modified.
     */
    const uchar* utf16() const;

    std::string to_utf8() const;

    static String from_utf8(const char* value) {
        return String(value);
    }

    static String from_utf8(const std::string& value) {
        return String(value);
    }

    static String number(int num);
    static String number(unsigned int num);
    static String number(long long num);
    static String number(double num);

    int length() const;
    
    /**
     * @brief split to substrings using @a sep as substring separator
     * @throw value_error if sep is empty
     */
    StringList split(const String& sep,
                     SplitPolicy split=KEEP_EMPTY_PARTS) const;

    /**
     * @brief add string @a str to the end of this string
     */
    String& append(const String& str);

    /**
     * @brief add UTF-16 code unit @a ch to the end of this string
     */
    String& append(uchar ch);
    
    /**
     * @brief prefix this string with string @a str
     */
    String& prepend(const String& str);
    
    /**
     * @brief insert string @a str into this string at index @a pos
     * @throw value_error if @a pos lies outside of string or is negative
     */
    String& insert(int pos, const String& str);

    /**
     * @brief remove @a n characters starting from position @a pos
     *
     * @throw value_error if @a pos lies outside fo string or is negative
     */
    String& remove(int pos, int n);

    /**
     * @brief remove all occurences of substring @a str
     */
    String& remove(const String& str);

    /**
     * @brief replace every occurence of @a before with @a after
     * if @a before is empty, string is left as it is
     */
    String& replace(const String& before, const String& after);

    /**
     * @brief replace @a n characters starting from @a pos with @a after
     *
     * @throw value_error if @a pos lies outside fo string or is negative
     */
    String& replace(int pos, int n, const String& after);

    /**
     * @brief find last index of substring @a str in this string
     * @param from position to search from, defaults to end of the string
     * @return the index position or -1 if not found
     *
     * @note from marks the last index that can be returned!
     */
    int last_index_of(const String& str, int from=-1) const;
    
    /**
     * @brief find first index of substring @a str in this string
     * @param from position to search from, defaults to the beginning of
     *        the string
     * @return the index position or -1 if not found
     *
     */
    int index_of(const String& str, int from=0) const;
    
    /**
     * @brief test if this string contains substring @a str
     */
    bool contains(const String& str) const;

    /**
     * @brief test if this string contains UTF-16 code unit @a ch
     */
    bool contains(uchar ch) const;
    
    /**
     * @brief test if this string starts with substring @a str
     */
    bool starts_with(const String& str) const;

    /**
     * @brief test if this string ends with substring @a str
     */
    bool ends_with(const String& str) const;

    String right_justified(int width, uchar ch) const;

    String section(const String& sep, int start, int end=-1) const;
        
    int to_int() const;
    
    /**
     * @throw value_error if @a pos is invalid
     */
    uchar char_at(int pos) const;
    
    /**
     * @brief extract a substring starting from @a pos with a span of @a len
     * @param pos starting position
     * @param len number of characters in the substring,
     *        if -1 (the default) all characters to the end of the string
     * @throw value_error if pos is outside of string
     */
    String substr(int pos, int length=-1) const;

    String to_lower() const;

    bool operator==(const String& rhs) const;

    bool operator!=(const String& rhs) const {
        return not (*this == rhs);
    }

    bool operator<(const String& rhs) const;
    
    String operator+(const String& rhs) const {
        return String(value_ + rhs.value_);
    }

    String& operator+=(const String& rhs) {
        value_ += rhs.value_;
        return *this;
    }

    String& operator+=(uchar ch) {
        value_ += ch;
        return *this;
    }

  private:
    UnicodeString value_;
};

String operator+(const char* s1, const String& s2);
bool operator==(const char* s1, const String& s2);

inline
bool operator!=(const char* s1, const String& s2) {
    return not (s1 == s2);
}

} // namespace brfc

std::ostream&
operator<<(std::ostream& out, const brfc::String& value);

#endif // BRFC_STRING_HPP
