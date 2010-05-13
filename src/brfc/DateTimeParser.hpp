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

#ifndef BRFC_DATE_TIME_PARSER_HPP
#define BRFC_DATE_TIME_PARSER_HPP

#include <boost/noncopyable.hpp>

#include <vector>

#include <brfc/smart_ptr.hpp>
#include <brfc/DateTime.hpp>
#include <brfc/String.hpp>

namespace brfc {

/**
 * @brief DateTimeParser tokens
 */
namespace dttok {

/**
 * @brief ABC for DateTimeParser tokens
 */
class Token : public boost::noncopyable {
  public:
    virtual ~Token() { }
    /**
     * @brief convert this token to string from DateTime @a dt
     */
    virtual String to_string(const DateTime& dt) const = 0;

    /**
     * @brief convert this token from string to datetime field
     * @param dt[out] object to set value on
     * @param str string to parse from
     * @return number of characters consumed (should be <= max_length())
     * @throw value_error
     */
    virtual int to_datetime(DateTime& dt, const String& str) const = 0;
    
    /**
     * @brief maximum length of this token
     */
    virtual int max_length() const = 0;
};

/**
 * @brief Token containing a String literal
 */
class Literal : public Token {
  public:
    explicit Literal(const String& str)
        : str_(str) {
    }
    
    virtual String to_string(const DateTime& d) const {
        return str_;
    }

    virtual int to_datetime(DateTime& dt, const String& str) const {
        return str_.length();
    }

    virtual int max_length() const { return str_.length(); }
    
    /**
     * @brief contained literal
     */
    const String& value() const {
        return str_;
    }

  private:
    String str_;
};

/**
 * @brief ABC for integer tokens
 */
class IntToken : public Token {
  public:
    /**
     * @brief constructor
     * @param zero_prefixed is the integer prefixed by zeroes
     * @param max_length maximum length of this token in string form
     */
    IntToken(int max_length, bool zero_prefixed, bool signed__=false)
            : max_length_(max_length)
            , zero_prefixed_(zero_prefixed)
            , signed_(signed__) {
    }

    virtual int max_length() const;

    virtual String to_string(const DateTime& dt) const;

    virtual int to_datetime(DateTime& dt, const String& str) const;  

  protected:
    virtual int get_value(const DateTime& dt) const = 0;
    virtual void set_value(DateTime& dt, int value) const = 0;
  
  private:
    int max_length_;
    bool zero_prefixed_;
    bool signed_;
};

/**
 * @brief marker class for Time specific tokens
 */
class TimeToken : public IntToken {
  public:
    TimeToken(int max_length, bool zero_prefixed, bool signed__=false)
            : IntToken(max_length, zero_prefixed, signed__) {
    }
};

/**
 * @brief marker class for Date specific tokens
 */
class DateToken : public IntToken {
  public:
    DateToken(int max_length, bool zero_prefixed, bool signed__=false)
        : IntToken(max_length, zero_prefixed, signed__) {
    }
};

class Hour : public TimeToken {
  public:
    explicit Hour(bool zero_prefixed)
        : TimeToken(2, zero_prefixed) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.time().hour();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.time().hour(value);
    }
};

class Minute : public TimeToken {
  public:
    explicit Minute(bool zero_prefixed)
            : TimeToken(2, zero_prefixed) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.time().minute();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.time().minute(value);
    }
};

class Second : public TimeToken {
  public:
    explicit Second(bool zero_prefixed)
            : TimeToken(2, zero_prefixed) {

    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.time().second();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.time().second(value);
    }
};

class Msec : public TimeToken {
  public:
    explicit Msec(bool zero_prefixed)
            : TimeToken(3, zero_prefixed) {

    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.time().msec();
    }

    virtual void set_value(DateTime& dt, int value) const {
        return dt.time().msec(value);
    }
};

class Year : public DateToken {
  public:
    Year()
            : DateToken(4, false, true) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.date().year();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.date().year(value);
    }
};

class ShortYear : public DateToken {
  public:
    ShortYear()
            : DateToken(2, false) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.date().year() - 2000;
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.date().year(value + 2000);
    }
};

class Month : public DateToken {
  public:
    Month(bool zero_prefixed)
            : DateToken(2, zero_prefixed) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.date().month();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.date().month(value);
    }
};

class Day : public DateToken {
  public:
    Day(bool zero_prefixed)
            : DateToken(2, zero_prefixed) {
    }
  
  protected:
    virtual int get_value(const DateTime& dt) const {
        return dt.date().day();
    }

    virtual void set_value(DateTime& dt, int value) const {
        dt.date().day(value);
    }
};

} // namespace dttok;

/**
 * @brief transform String to DateTime and vice-versa
 */
class DateTimeParser {
  public:
    typedef std::vector<shared_ptr<dttok::Token> > TokenVector;
    
    /**
     * @brief construct from format string
     */ 
    explicit DateTimeParser(const String& format);

    /**
     * @brief construct from a vector of tokens
     */
    explicit DateTimeParser(const TokenVector& format);

    ~DateTimeParser();

    /**
     * @brief test if the parser contains tokens valid for Date
     */
    bool is_format_date_only() const;
    
    /**
     * @brief test if the parser contains tokens valid for Time
     */
    bool is_format_time_only() const;

    DateTime from_string(const String& str);
    String to_string(const DateTime& dt);

    Date date_from_string(const String& str);
    String date_to_string(const Date& date);

    Time time_from_string(const String& str);
    String time_to_string(const Time& time);

    /**
     * @brief tokenize a format string
     *
     * recognized date tokens:
     *  - @c yyyy - 4-digit year
     *  - @c yy - 2-digit year
     *  - @c MM - month number with a leading zero (01-12)
     *  - @c M - month number without a leading zero (1-12)
     *  - @c dd - day number with a leading zero (01-31)
     *  - @c d - day number without a leading zero (1-31)
     * 
     * recognized time tokens:
     *  - @c hh or @c HH - hour with a leading zero (00-23)
     *  - @c h or @c H - hour without a leading zero (0-23)
     *  - @c mm - minute with a leading zero (00-59)
     *  - @c m - minute without a leading zero (0-59)
     *  - @c ss - second with a leading zero (00-59)
     *  - @c s - second without a leading zero (0-59)
     *  - @c zzz - millisecond with leading zeroes (000-999)
     *  - @c z - millisecond without leading zeroes (0-999)
     *
     * all other characters and text between single quotes (') are treated
     * as string literals. Double single quotes ('') are treated as a
     * literal single quote.
     */
    static TokenVector tokenize(const String& format);
    
    static shared_ptr<dttok::Token> token_from_string(const String& str);

  private:
    TokenVector tokens_; 
};

} // namespace brfc

#endif // BRFC_DATE_TIME_PARSER_HPP
