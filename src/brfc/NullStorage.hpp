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

#ifndef BRFC_NULL_STORAGE_HPP
#define BRFC_NULL_STORAGE_HPP

#include <brfc/LocalStorage.hpp>

namespace brfc {

class NullStorage : public LocalStorage {
  protected:
    /**
     * @return always null pointer
     */
    virtual shared_ptr<const oh5::File> do_prestore(const FileEntry&) {
        return shared_ptr<const oh5::File>();
    }
    
    /**
     * @return always null pointer
     */
    virtual shared_ptr<const oh5::File> do_store(const FileEntry&) {
        return shared_ptr<const oh5::File>();
    }
    
    /**
     * @return always true
     */
    virtual bool do_remove(const FileEntry&) {
        return true;
    }

    virtual void do_clean() { }
};

} // namespace brfc

#endif // BRFC_NULL_STORAGE_HPP
