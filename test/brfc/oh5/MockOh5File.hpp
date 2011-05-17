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
#ifndef BRFC_OH5_MOCK_FILE_HPP
#define BRFC_OH5_MOCK_FILE_HPP

#include <gmock/gmock.h>

#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Node.hpp>

namespace brfc {

class MockOh5File : public Oh5File {
  public:
    MOCK_CONST_METHOD0(do_root, const Oh5Node&());
};

} // namespace brfc

#endif // BRFC_OH5_MOCK_FILE_HPP