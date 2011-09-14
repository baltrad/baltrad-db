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

#include <brfc/oh5/Oh5File.hpp>

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5Group.hpp>

namespace brfc {

Oh5File::Oh5File()
        : metadata_()
        , path_() {
}

Oh5File::Oh5File(const std::string& object,
                 const Date& date,
                 const Time& time,
                 const std::string& source,
                 const std::string& version)
        : metadata_()
        , path_() {
    Oh5Node& root = metadata().root();
    root.add(new Oh5Attribute("Conventions", Oh5Scalar("ODIM_H5/V2_0")));
    Oh5Node& what = root.add(new Oh5Group("what"));
    what.add(new Oh5Attribute("object", Oh5Scalar(object)));
    what.add(new Oh5Attribute("version", Oh5Scalar(version)));
    what.add(new Oh5Attribute("date", Oh5Scalar(date)));
    what.add(new Oh5Attribute("time", Oh5Scalar(time)));
    what.add(new Oh5Attribute("source", Oh5Scalar(source)));
}

Oh5File::~Oh5File() {

}

std::string
Oh5File::name() const {
    const std::string& p = path();
    size_t idx = p.rfind('/');
    return idx == std::string::npos ? p : p.substr(idx + 1);
}

} // namepsace brfc
