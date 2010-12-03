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

#ifndef BRFC_SQL_DIALECT_HPP
#define BRFC_SQL_DIALECT_HPP

#include <string>

namespace brfc {

class Variant;

namespace sql {

class Dialect {
  public:
    enum Feature {
        RETURNING = 1,
        LAST_INSERT_ID = 2
    };

    virtual ~Dialect() { }

    bool has_feature(Feature feature) const {
        return do_has_feature(feature);
    }

    const std::string& name() const {
        return do_name();
    }

    /**
     * @sa do_variant_to_string
     */
    std::string variant_to_string(const Variant& value) const {
        return do_variant_to_string(value);
    }

  protected:
    /**
     * @return false
     */
    virtual bool do_has_feature(Feature feature) const = 0;

    virtual const std::string& do_name() const = 0;

    /**
     * @brief default implementation
     * 
     * - date and time in ISO 8601 format
     * - bool values as TRUE or FALSE
     * - none as NULL
     * - string surrounded by apostrophes (')
     */
    virtual std::string do_variant_to_string(const Variant& value) const;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_DIALECT_HPP
