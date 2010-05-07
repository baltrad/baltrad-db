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

#ifndef BRFC_OH5_SOURCE_H
#define BRFC_OH5_SOURCE_H

#include <brfc/smart_ptr.hpp>
#include <brfc/String.hpp>

namespace brfc {
namespace oh5 {

class Source : public enable_shared_from_this<Source> {
  public:
    virtual ~Source() { }

    /**
     * @brief construct from ODIM_H5 '/what/source' metadata attribute
     *
     * @param source contents of '/what/source'
     * @throw value_error when source contains invalid fields or not
     *        correctly formed
     *
     * valid elements are [WMO,RAD,ORG,PLC,CTY,CMT]
     * string should consist of element-value pairs, where
     * element and value are separated by a colon.
     * pairs are separated by comma.
     *
     */
    static shared_ptr<Source> from_source_attribute(const String& source);

    String node_id() const { return node_id_; }

    void node_id(const String& value) {
        node_id_ = value;
    }

    virtual String wmo_cccc() const = 0;

    virtual String to_string() const = 0;
  
  protected:
    explicit Source(const String& node_id="")
             : node_id_(node_id) {
    }

  private:
    String node_id_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_H
