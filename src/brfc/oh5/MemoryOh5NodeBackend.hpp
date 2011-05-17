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

#ifndef BRFC_OH5_MEMORY_NODE_BACKEND_HPP
#define BRFC_OH5_MEMORY_NODE_BACKEND_HPP

#include <brfc/oh5/Oh5NodeBackend.hpp>

#include <boost/scoped_ptr.hpp>

namespace brfc {

/**
 * @brief NodeBackend storing nodes only in memory
 */
class MemoryOh5NodeBackend : public Oh5NodeBackend {
  public:
    /**
     * @brief constructor
     */
    MemoryOh5NodeBackend();

    virtual ~MemoryOh5NodeBackend();
  
  private:

    virtual Oh5Node& do_add(Oh5Node* node);

    virtual bool do_has(const Oh5Node& node) const;

    virtual const Oh5Node& do_root() const;

    virtual std::vector<const Oh5Node*> do_children(const Oh5Node& node) const;

    struct Impl;
    boost::scoped_ptr<Impl> impl_;
};

} // namespace brfc

#endif // BRFC_OH5_MEMORY_NODE_BACKEND_HPP
