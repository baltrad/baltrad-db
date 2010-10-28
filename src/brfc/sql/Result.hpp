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

#ifndef BRFC_SQL_RESULT_HPP
#define BRFC_SQL_RESULT_HPP

#include <boost/noncopyable.hpp>

#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

/**
 * @brief ABC for database results
 */
class Result : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~Result() { }

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
     * @brief get value at @c pos as Variant
     * @throw lookup_error if @c pos is out of range
     */
    Variant value_at(unsigned int pos) const {
        return do_value_at(pos);
    }
    
    /**
     * @brief get value in @c column as Variant
     * @throw lookup_error if @c column is not in result
     */
    Variant value_at(const String& column) const {
        return do_value_at(column);
    }
    
    /**
     * @brief number of rows the executed statement affected
     */
    int affected_rows() const {
        return do_affected_rows();
    }


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
     * @brief value_at(unsigned int) implementation
     */
    virtual Variant do_value_at(unsigned int pos) const = 0;

    /**
     * @brief value_at(const String&) implementation
     */
    virtual Variant do_value_at(const String& column) const = 0;

    virtual int do_affected_rows() const = 0;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_RESULT_HPP
