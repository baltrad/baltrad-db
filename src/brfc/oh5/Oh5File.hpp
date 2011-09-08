/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_OH5_FILE_HPP
#define BRFC_OH5_FILE_HPP

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

namespace brfc {

class Date;
class Oh5Attribute;
class Oh5Group;
class Oh5Node;
class Oh5Source;
class Time;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 *
 * @ingroup exposed
 */
class Oh5File : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~Oh5File() { }
    
    /**
     * @brief get hold of Node at path
     * @param path path to Node, where elements are separated by '/'
     * @return pointer to requested Node if found
     *
     * group("path/to/node")
     */
    const Oh5Node* node(const std::string& path) const;
    
    /**
     * @copydoc node(const String&) const
     */
    Oh5Node* node(const std::string& path);

    /**
     * @brief get hold of Attribute at @c path
     * @param path path to Attribute, where elements are separated by '/'
     * @return pointer to requested Attribute if found
     */
    const Oh5Attribute* attribute(const std::string& path) const;

    /**
     * @brief get hold of Attribute at @c path
     * @param path path to Attribute, where elements are separated by '/'
     * @return pointer to requested Attribute if found
     */
    Oh5Attribute* attribute(const std::string& path);

    /**
     * @brief get hold of Group at path
     * @param path path to Group, where elements are separated by '/'
     * @return pointer to requested Group if found
     *
     * group("path/to/group")
     */
    const Oh5Group* group(const std::string& path) const;
    
    /**
     * @copydoc group(const String&) const
     */
    Oh5Group* group(const std::string& path);
    
    /**
     * @brief get the root group
     */
    const Oh5Node& root() const { return do_root(); }
    
    /**
     * @copydoc root() const
     */
    Oh5Node& root() { return const_cast<Oh5Node&>(do_root()); }
    
    /**
     * @brief get Source associated with file
     */
    Oh5Source source() const;
    
    /**
     * @brief access /what/object
     * @throw lookup_error if attribute is missing
     * @throw std::invalid_argument if attribute value is not a std::string
     */
    std::string what_object() const;
    
    /**
     * @brief set /what/object, creating /what group if missing
     */
    void what_object(const std::string& value);

    /**
     * @brief access /what/date
     * @throw lookup_error if attribute is missing
     * @throw std::invalid_argument if attribute value is not a Date or a std::string in
     *                    'yyyyMMdd' format
     */
    Date what_date() const;

    /**
     * @brief set /what/date, creating /what group if missing
     */
    void what_date(const Date& value);

    /**
     * @brief access /what/time
     * @throw lookup_error if attribute is missing
     * @throw std::invalid_argument if attribute value is not a Time or a std::string in
     *                    'hhmmss' format
     */
    Time what_time() const;

    /**
     * @brief set /what/time, creating /what group if missing
     */
    void what_time(const Time& value);

    /**
     * @brief access /what/source
     *
     * @throw lookup_error if attribute is missing
     * @throw std::invalid_argument if attribute value is not a std::string
     */
    std::string what_source() const;

    /**
     * @brief set /what/source, creating /what group if missing
     */
    void what_source(const std::string& value);
  
  protected:
    /**
     * @brief root() implementation
     */
    virtual const Oh5Node& do_root() const = 0;

    /**
     * @brief source() default implementation
     * @throw std::invalid_argument if /what/source is not correctly formed
     * @return Source parsed from /what/source or empty Source if
     *         /what/source is missing
     */
    virtual Oh5Source do_source() const;
};

} // namespace brfc
#endif // BRFC_OH5_FILE_HPP
