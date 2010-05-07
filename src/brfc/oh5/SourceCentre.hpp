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

#ifndef BRFC_OH5_SOURCE_CENTRE_HPP
#define BRFC_OH5_SOURCE_CENTRE_HPP

#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

class SourceCentre : public Source {
  public:
    /**
     * @brief originating centre
     */
    int originating_centre() const { return originating_centre_; }
    
    void originating_centre(int value) {
        originating_centre_ = value;
    }

    /**
     * @brief country code according to BUFR tables
     */
    int country_code() const { return country_code_; }

    void country_code(int value) {
        country_code_ = value;
    }

    /**
     * @brief WMO CCCC from Volume C1
     */
    virtual String wmo_cccc() const { return wmo_cccc_; }

    void wmo_cccc(const String& value) {
        wmo_cccc_ = value;
    }

    virtual String to_string() const;
  
  protected:
    template<class T> 
    friend 
    shared_ptr<T> boost::make_shared();

    SourceCentre()
            : Source()
            , originating_centre_(0)
            , country_code_(0)
            , wmo_cccc_() {

    }

  private:
    int originating_centre_;
    int country_code_;
    String wmo_cccc_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_CENTRE_HPP
