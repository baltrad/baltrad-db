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

#include <string>

#include <boost/noncopyable.hpp>

namespace brfc {

class Date;
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
    int size() const {
        return do_size();
    }
    //@}
    
    /**
     * @brief test if value at \c name is NULL
     */
    bool is_null(const std::string& name) const;
    
    /**
     * @name value access
     * @{
     */
    /**
     * @brief get value at @c name as Variant
     * @throw lookup_error if result doesn't contain a value by @c name
     */
    Variant value_at(const std::string& name) const;

    /**
     * @brief get string value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_string(); @endcode
     */
    std::string string(const std::string& name) const;

    /**
     * @brief get a 64-bit integer value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_int64(); @endcode
     */
    long long int64_(const std::string& name) const;

    /**
     * @brief get a double precision floating-point value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_double(); @endcode
     */
    double double_(const std::string& name) const;

    /**
     * @brief get boolean value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_bool(); @endcode
     */
    bool bool_(const std::string& name) const;

    /**
     * @brief get date value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_date(); @endcode
     */
    Date date(const std::string& name) const;

    /**
     * @brief get time value at \c name
     * @throw lookup_error if result doesn't contain a value by @c name
     *
     * equivalent to: @code value_at(name).to_time(); @endcode
     */
    Time time(const std::string& name) const;    
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
    virtual int do_size() const = 0;

    /**
     * @brief value_at() implementation
     */
    virtual Variant do_value_at(const std::string& name) const = 0;
};

} // namespace db
} // namespace brfc

#endif // BRFC_DB_ATTRIBUTE_RESULT_HPP
