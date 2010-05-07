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

#ifndef BRFC_OH5_SOURCE_RADAR_HPP
#define BRFC_OH5_SOURCE_RADAR_HPP

#include <brfc/assert.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceCentre.hpp>

namespace brfc {
namespace oh5 {

class SourceRadar : public Source {
  public:
    /**
     * @brief combined WMO block and station number
     */
    int wmo_code() const { return wmo_code_; }

    void wmo_code(int value) {
        wmo_code_ = value;
    }

    /**
     * @brief radar site as indexed in the OPERA database
     */
    const String& radar_site() const { return radar_site_; }

    void radar_site(const String& value) {
        radar_site_ = value;
    }

    /**
     * @brief place according to ODIM 2.0 spec, table 9
     */
    const String& place() const { return place_; }

    void place(const String& value) {
        place_ = value;
    }

    shared_ptr<SourceCentre> centre() const { return centre_; }

    void centre(shared_ptr<SourceCentre> value) {
        centre_= value;
    }

    virtual String wmo_cccc() const {
        BRFC_ASSERT(centre_);
        return centre_->wmo_cccc();
    }

    virtual String to_string() const;
  
  protected:
    template<class T> 
    friend 
    shared_ptr<T> boost::make_shared();

    SourceRadar()
            : wmo_code_(0)
            , radar_site_("")
            , place_("")
            , centre_() {
    }
   
  private:
    int wmo_code_;
    String radar_site_;
    String place_;
    shared_ptr<SourceCentre> centre_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_RADAR_HPP
