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

#ifndef BRFC_RDB_MODEL_HPP
#define BRFC_RDB_MODEL_HPP

#include <map>

#include <brfc/String.hpp>
#include <brfc/sql/fwd.hpp>

namespace brfc {
namespace rdb {

class Model {
  public:
    static Model& instance();

    sql::TablePtr sources;
    sql::TablePtr source_radars;
    sql::TablePtr source_centres;
    sql::TablePtr files;
    sql::TablePtr file_content;
    sql::TablePtr groups;
    sql::TablePtr attrs;
    sql::TablePtr invalid_attrs;
    sql::TablePtr attrvals_int;
    sql::TablePtr attrvals_str;
    sql::TablePtr attrvals_real;
    sql::TablePtr attrvals_bool;

    sql::TablePtr table_by_name(const String& name) const;

  private:
    Model();

    typedef std::map<String, sql::TablePtr> TableMap;
    
    TableMap tables_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_MODEL_HPP
