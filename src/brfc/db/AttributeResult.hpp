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

#ifndef BRFC_DB_ATTRIBUTE_RESULT_HPP
#define BRFC_DB_ATTRIBUTE_RESULT_HPP

#include <boost/noncopyable.hpp>

#include <brfc/deprecate.hpp>

namespace brfc {

class Date;
class String;
class Time;
class Variant;

namespace db {

/**
 * @brief ABC for Query results
 */
class AttributeResult : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~AttributeResult() { }

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
    
    /**
     * @brief test if value at \c pos is NULL
     */
    bool is_null(unsigned int pos) const;
    
    /**
     * @name value access
     * @{
     */
    /**
     * @brief get value at @c pos as Variant
     * @throw lookup_error if @c pos is out of range
     */
    Variant value_at(unsigned int pos) const;

    /**
     * @brief get string value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_string(); @endcode
     */
    String string(unsigned int pos) const;

    /**
     * @brief get a 64-bit integer value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_int64(); @endcode
     */
    long long int64_(unsigned int pos) const;

    /**
     * @brief get a double precision floating-point value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_double(); @endcode
     */
    double double_(unsigned int pos) const;

    /**
     * @brief get boolean value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_bool(); @endcode
     */
    bool bool_(unsigned int pos) const;

    /**
     * @brief get date value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_date(); @endcode
     */
    Date date(unsigned int pos) const;

    /**
     * @brief get time value at \c pos
     * @throw lookup_error if @c pos is out of range
     *
     * equivalent to: @code value_at(pos).to_time(); @endcode
     */
    Time time(unsigned int pos) const;    
    //@}
     
  
  protected:
    /**
     * @brief next() implementation
     */
    virtual bool do_next() = 0;

    /**
     * @brief seek() implementation
     */
    virtual bool do_seek(int idx) = 0;

    /**
     * @brief size() implementation
     */
    virtual int do_size() = 0;

    /**
     * @brief value_at() implementation
     */
    virtual Variant do_value_at(unsigned int pos) const = 0;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_ATTRIBUTE_RESULT_HPP
