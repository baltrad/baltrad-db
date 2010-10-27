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

#include <brfc/db/rdb/Model.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace db {
namespace rdb {

Model::Model()
        : sources(sql::Table::create("bdb_sources"))
        , source_kvs(sql::Table::create("bdb_source_kvs"))
        , files(sql::Table::create("bdb_files"))
        , file_content(sql::Table::create("bdb_file_content"))
        , nodes(sql::Table::create("bdb_nodes"))
        , attrs(sql::Table::create("bdb_attributes"))
        , attrvals(sql::Table::create("bdb_attribute_values")) {
    sources->add_column("id");
    sources->add_column("name");
    
    source_kvs->add_column("source_id");
    source_kvs->add_column("key");
    source_kvs->add_column("value");
    source_kvs->column("source_id")->references(sources->column("id"));

    files->add_column("id");
    files->add_column("uuid");
    files->add_column("hash");
    files->add_column("stored_at");
    files->add_column("object");
    files->add_column("n_date");
    files->add_column("n_time");
    files->add_column("source_id");
    files->column("source_id")->references(sources->column("id"));

    file_content->add_column("file_id");
    file_content->add_column("lo_id");
    file_content->column("file_id")->references(files->column("id"));

    nodes->add_column("id");
    nodes->add_column("parent_id");
    nodes->add_column("name");
    nodes->add_column("type");
    nodes->add_column("file_id");
    nodes->column("file_id")->references(files->column("id"));

    attrs->add_column("id");
    attrs->add_column("name");
    attrs->add_column("converter");
    attrs->add_column("storage_table");
    attrs->add_column("storage_column");
    attrs->add_column("ignore_in_hash");

    attrvals->add_column("node_id");
    attrvals->add_column("value_int");
    attrvals->add_column("value_str");
    attrvals->add_column("value_real");
    attrvals->add_column("value_bool");
    attrvals->add_column("value_date");
    attrvals->add_column("value_time");
    attrvals->column("node_id")->references(nodes->column("id"));

    tables_.insert(std::make_pair(sources->name(), sources));
    tables_.insert(std::make_pair(source_kvs->name(), source_kvs));
    tables_.insert(std::make_pair(files->name(), files));
    tables_.insert(std::make_pair(nodes->name(), nodes));
    tables_.insert(std::make_pair(attrvals->name(), attrvals));
}

sql::TablePtr
Model::table_by_name(const String& name) const {
    TableMap::const_iterator iter = tables_.find(name);
    if (iter == tables_.end())
        throw lookup_error(name.to_std_string());
    return iter->second;
}

Model&
Model::instance() {
    static Model model;
    return model;
}

} // namespace rdb
} // namespace db
} // namespace brfc
