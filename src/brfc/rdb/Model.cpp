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

#include <brfc/rdb/Model.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Column.hpp>
#include <brfc/sql/Table.hpp>

namespace brfc {
namespace rdb {

Model::Model()
        : sources(sql::Table::create("bdb_sources"))
        , source_radars(sql::Table::create("bdb_source_radars"))
        , source_centres(sql::Table::create("bdb_source_centres"))
        , files(sql::Table::create("bdb_files"))
        , file_content(sql::Table::create("bdb_file_content"))
        , groups(sql::Table::create("bdb_groups"))
        , attrs(sql::Table::create("bdb_attributes"))
        , invalid_attrs(sql::Table::create("bdb_invalid_attributes"))
        , attrvals_int(sql::Table::create("bdb_attribute_values_int"))
        , attrvals_str(sql::Table::create("bdb_attribute_values_str"))
        , attrvals_real(sql::Table::create("bdb_attribute_values_real"))
        , attrvals_bool(sql::Table::create("bdb_attribute_values_bool")) {

    sources->add_column("id");
    sources->add_column("node_id");

    source_centres->add_column("id");
    source_centres->add_column("originating_centre");
    source_centres->add_column("country_code");
    source_centres->add_column("wmo_cccc");
    source_centres->column("id")->references(sources->column("id"));

    source_radars->add_column("id");
    source_radars->add_column("centre_id");
    source_radars->add_column("radar_site");
    source_radars->add_column("wmo_code");
    source_radars->add_column("place");
    source_radars->column("id")->references(sources->column("id"));

    files->add_column("id");
    files->add_column("hash_type");
    files->add_column("unique_id");
    files->add_column("object");
    files->add_column("n_date");
    files->add_column("n_time");
    files->add_column("source_id");
    files->column("source_id")->references(sources->column("id"));

    file_content->add_column("file_id");
    file_content->add_column("lo_id");

    groups->add_column("id");
    groups->add_column("parent_id");
    groups->add_column("name");
    groups->add_column("product");
    groups->add_column("startdate");
    groups->add_column("starttime");
    groups->add_column("enddate");
    groups->add_column("endtime");
    groups->add_column("file_id");
    groups->column("file_id")->references(files->column("id"));

    attrs->add_column("id");
    attrs->add_column("name");
    attrs->add_column("converter");
    attrs->add_column("storage_table");
    attrs->add_column("storage_column");
    attrs->add_column("ignore_in_hash");

    invalid_attrs->add_column("name");
    invalid_attrs->add_column("group_id");
    invalid_attrs->column("group_id")->references(groups->column("id"));

    attrvals_int->add_column("attribute_id");
    attrvals_int->add_column("group_id");
    attrvals_int->add_column("value");
    attrvals_int->column("attribute_id")->references(attrs->column("id"));
    attrvals_int->column("group_id")->references(groups->column("id"));

    attrvals_str->add_column("attribute_id");
    attrvals_str->add_column("group_id");
    attrvals_str->add_column("value");
    attrvals_str->column("attribute_id")->references(attrs->column("id"));
    attrvals_str->column("group_id")->references(groups->column("id"));

    attrvals_real->add_column("attribute_id");
    attrvals_real->add_column("group_id");
    attrvals_real->add_column("value");
    attrvals_real->column("attribute_id")->references(attrs->column("id"));
    attrvals_real->column("group_id")->references(groups->column("id"));

    attrvals_bool->add_column("attribute_id");
    attrvals_bool->add_column("group_id");
    attrvals_bool->add_column("value");
    attrvals_bool->column("attribute_id")->references(attrs->column("id"));
    attrvals_bool->column("group_id")->references(groups->column("id"));

    tables_.insert(std::make_pair(sources->name(), sources));
    tables_.insert(std::make_pair(source_radars->name(), source_radars));
    tables_.insert(std::make_pair(source_centres->name(), source_centres));
    tables_.insert(std::make_pair(files->name(), files));
    tables_.insert(std::make_pair(groups->name(), groups));
    tables_.insert(std::make_pair(attrvals_int->name(), attrvals_int));
    tables_.insert(std::make_pair(attrvals_str->name(), attrvals_str));
    tables_.insert(std::make_pair(attrvals_real->name(), attrvals_real));
    tables_.insert(std::make_pair(attrvals_bool->name(), attrvals_bool));

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
} // namespace brfc
