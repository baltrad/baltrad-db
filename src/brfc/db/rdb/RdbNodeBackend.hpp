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

#ifndef BRFC_DB_RDB_RDB_NODE_BACKEND_HPP
#define BRFC_DB_RDB_RDB_NODE_BACKEND_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include <string>

#include <brfc/oh5/NodeBackend.hpp>


namespace brfc {
namespace db {
namespace rdb {

class RelationalDatabase;

class RdbNodeBackend : public oh5::NodeBackend {
  public:
    explicit RdbNodeBackend(long long id=0);

    virtual ~RdbNodeBackend();

    void id(long long id) { id_ = id; }
    
    long long id() const { return id_; }

    bool loaded() const { return loaded_; }

    void loaded(bool loaded) { loaded_ = loaded; }

  protected:
    virtual oh5::Node& do_create_child(oh5::Node* node);

    virtual std::vector<oh5::Node*> do_children();
    virtual std::vector<const oh5::Node*> do_children() const;

    RelationalDatabase& rdb() const;

  private:
    void load() const;

    boost::ptr_vector<oh5::Node> children_;
    long long id_;
    mutable bool loaded_;
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_RDB_NODE_BACKEND_HPP
