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

#include <vector>

#include <boost/noncopyable.hpp>

#include <string>

namespace brfc {

class Date;
class Time;

namespace oh5 {

class Attribute;
class Group;
class Node;
class Source;

/**
 * @brief a HDF5 file conforming to ODIM_H5/V2_0 specification
 */
class File : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~File() { }
    
    /**
     * @brief get hold of Node at path
     * @param path path to Node, where elements are separated by '/'
     * @return pointer to requested Node if found
     *
     * group("path/to/node")
     */
    const Node* node(const std::string& path) const;
    
    /**
     * @copydoc node(const String&) const
     */
    Node* node(const std::string& path);

    /**
     * @brief get hold of Group at path
     * @param path path to Group, where elements are separated by '/'
     * @return pointer to requested Group if found
     *
     * group("path/to/group")
     */
    const Group* group(const std::string& path) const;
    
    /**
     * @copydoc group(const String&) const
     */
    Group* group(const std::string& path);
    
    /**
     * @brief get the root group
     */
    const Group& root() const { return do_root(); }
    
    /**
     * @copydoc root() const
     */
    Group& root() { return const_cast<Group&>(do_root()); }
    
    /**
     * @brief get Source associated with file
     */
    Source source() const;
    
    /**
     * @name mandatory attribute access shorthands
     * @{
     */
    /**
     * @brief access /what/object
     * @throw lookup_error if attribute is missing
     * @throw value_error if attribute value is not a std::string
     */
    std::string what_object() const;

    /**
     * @brief access /what/date
     * @throw lookup_error if attribute is missing
     * @throw value_error if attribute value is not a Date or a std::string in
     *                    'yyyyMMdd' format
     */
    Date what_date() const;

    /**
     * @brief access /what/time
     * @throw lookup_error if attribute is missing
     * @throw value_error if attribute value is not a Time or a std::string in
     *                    'hhmmss' format
     */
    Time what_time() const;

    /**
     * @brief access /what/source
     *
     * @throw lookup_error if attribute is missing
     * @throw value_error if attribute value is not a std::string
     */
    std::string what_source() const;
    ///@}
  
  protected:
    /**
     * @brief root() implementation
     */
    virtual const Group& do_root() const = 0;

    /**
     * @brief source() default implementation
     * @throw value_error if /what/source is not correctly formed
     * @return Source parsed from /what/source or empty Source if
     *         /what/source is missing
     */
    virtual Source do_source() const;
};

} // namespace oh5
} // namespace brfc
#endif // BRFC_OH5_FILE_HPP
