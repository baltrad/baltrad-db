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
#ifndef BRFC_OH5_FAKE_FILE_HPP
#define BRFC_OH5_FAKE_FILE_HPP

#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/MemoryOh5NodeBackend.hpp>

namespace brfc {
namespace oh5 {

class FakeOh5File : public Oh5File {
  public:
    FakeOh5File()
            : backend_() {
    }

  private:
    virtual const Oh5Node& do_root() const { return backend_.root(); }

    MemoryOh5NodeBackend backend_;
};

} // namespace oh5
} // namepsace brfc


#endif // BRFC_OH5_FAKE_FILE_HPP
