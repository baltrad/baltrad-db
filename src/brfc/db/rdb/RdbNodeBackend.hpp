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

#ifndef BRFC_DB_RDB_RDB_NODE_BACKEND_HPP
#define BRFC_DB_RDB_RDB_NODE_BACKEND_HPP

#include <brfc/oh5/NodeBackend.hpp>
#include <boost/scoped_ptr.hpp>

namespace brfc {

class RelationalDatabase;

class RdbNodeBackend : public oh5::NodeBackend {
  public:
    explicit RdbNodeBackend(RelationalDatabase* rdb);

    virtual ~RdbNodeBackend();

    void id(const oh5::Node& node, long long id);
    
    long long id(const oh5::Node& node) const;

    bool loaded(const oh5::Node& node) const;

    void loaded(const oh5::Node& node, bool loaded);

  private:
    virtual oh5::Node& do_add(oh5::Node* node);

    virtual bool do_has(const oh5::Node& node) const;

    virtual const oh5::Node& do_root() const;

    virtual std::vector<const oh5::Node*>
        do_children(const oh5::Node& node) const;

    struct Impl;
    const boost::scoped_ptr<Impl> impl_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_RDB_NODE_BACKEND_HPP
