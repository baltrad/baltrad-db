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

#ifndef BRFC_EXCEPTIONS_HPP
#define BRFC_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace brfc {

/**
 * @defgroup exceptions Exceptions
 * @{
 */
/**
 * @brief base class for all errors from this library
 *
 * equivalent exception in java bindings is @c FileCatalogError
 */
class brfc_error : public std::runtime_error {
  public:
    explicit brfc_error(const std::string& what)
            : runtime_error(what) {
    }
};

/**
 * @brief invalid value supplied
 */
class value_error : public brfc_error {
  public:
    explicit value_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief object lookup failed
 *
 * equivalent exception in java bindings is @c LookupError
 */
class lookup_error : public brfc_error {
  public:
    explicit lookup_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief entry already exists
 *
 * equivalent exception in java bindings is @c DuplicateEntry
 */
class duplicate_entry : public brfc_error {
  public:
    explicit duplicate_entry(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief database query/connection failed
 *
 * equivalent exception in java bindings is @c DatabaseError
 */
class db_error : public brfc_error {
  public:
    explicit db_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief filesystem related error
 *
 * equivalent exception in java bindings is @c FileSystemError
 */
class fs_error : public brfc_error {
  public:
    explicit fs_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @} // end of exception group
 */

}

#endif // BRFC_EXCEPTIONS_HPP
