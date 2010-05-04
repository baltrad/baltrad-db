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

#ifndef BRFC_RESULT_SET_HPP
#define BRFC_RESULT_SET_HPP

#include <boost/noncopyable.hpp>

class QString;
class QDate;

namespace brfc {

class Time;
class Variant;

/**
 * @brief ABC for Query results
 */
class ResultSet : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~ResultSet() { }

    /**
     * @name seeking
     * @{
     */
    /**
     * @brief move to next row
     * @return true if new row is valid
     */
    bool next() {
        return do_next();
    }
    
    /**
     * @brief seek to row
     * @param idx row number, starting from 0. if negative, before first row.
     * @return true if new row is valid
     */
    bool seek(int idx) {
        return do_seek(idx);
    }

    /**
     * @brief get number of rows in result
     */
    int size() {
        return do_size();
    }
    //@}
    
    bool is_null(unsigned int pos) const;
    
    /**
     * @name value access
     * @{
     */
    /**
     * @brief get string value at \c pos
     */
    QString string(unsigned int pos) const;

    /**
     * @brief get integer value at \c pos
     */
    long long integer(unsigned int pos) const;

    /**
     * @brief get real value at \c pos
     */
    double real(unsigned int pos) const;

    /**
     * @brief get boolean value at \c pos
     */
    bool boolean(unsigned int pos) const;

    /**
     * @brief get date value at \c pos
     */
    QDate date(unsigned int pos) const;

    /**
     * @brief get time value at \c pos
     */
    Time time(unsigned int pos) const;
    //@}
  
  protected:
    virtual bool do_next() = 0;
    virtual bool do_seek(int idx) = 0;
    virtual int do_size() = 0;

    virtual bool do_is_null(unsigned int pos) const = 0;
    
    virtual QString do_string(unsigned int pos) const = 0;
    virtual long long do_integer(unsigned int pos) const = 0;
    virtual double do_real(unsigned int pos) const = 0;
    virtual bool do_boolean(unsigned int pos) const = 0;
    virtual QDate do_date(unsigned int pos) const = 0;
    virtual Time do_time(unsigned int pos) const = 0;

};

}

#endif // BRFC_RESULT_SET_HPP
