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
        , groups(sql::Table::create("bdb_groups"))
        , invalid_attrs(sql::Table::create("bdb_invalid_attributes"))
        , attrvals_int(sql::Table::create("bdb_attribute_values_int"))
        , attrvals_str(sql::Table::create("bdb_attribute_values_str"))
        , attrvals_real(sql::Table::create("bdb_attribute_values_real"))
        , attrvals_bool(sql::Table::create("bdb_attribute_values_bool")) {

    sources->add_column(sql::Column::create("id"));
    sources->add_column(sql::Column::create("node_id"));

    source_centres->add_column(sql::Column::create("id"));
    source_centres->add_column(sql::Column::create("originating_centre"));
    source_centres->add_column(sql::Column::create("country_code"));
    source_centres->add_column(sql::Column::create("wmo_cccc"));

    source_radars->add_column(sql::Column::create("id"));
    source_radars->add_column(sql::Column::create("centre_id"));
    source_radars->add_column(sql::Column::create("radar_site"));
    source_radars->add_column(sql::Column::create("wmo_code"));
    source_radars->add_column(sql::Column::create("place"));


    files->add_column(sql::Column::create("id"));
    files->add_column(sql::Column::create("hash_type"));
    files->add_column(sql::Column::create("unique_id"));
    files->add_column(sql::Column::create("path"));
    files->add_column(sql::Column::create("proposed_filename"));
    files->add_column(sql::Column::create("filename_version"));
    files->add_column(sql::Column::create("object"));
    files->add_column(sql::Column::create("n_date"));
    files->add_column(sql::Column::create("n_time"));
    files->add_column(sql::Column::create("source_id"));

    groups->add_column(sql::Column::create("id"));
    groups->add_column(sql::Column::create("parent_id"));
    groups->add_column(sql::Column::create("name"));
    groups->add_column(sql::Column::create("product"));
    groups->add_column(sql::Column::create("startdate"));
    groups->add_column(sql::Column::create("starttime"));
    groups->add_column(sql::Column::create("enddate"));
    groups->add_column(sql::Column::create("endtime"));
    groups->add_column(sql::Column::create("file_id"));

    invalid_attrs->add_column(sql::Column::create("name"));
    invalid_attrs->add_column(sql::Column::create("group_id"));

    attrvals_int->add_column(sql::Column::create("attribute_id"));
    attrvals_int->add_column(sql::Column::create("group_id"));
    attrvals_int->add_column(sql::Column::create("value"));

    attrvals_str->add_column(sql::Column::create("attribute_id"));
    attrvals_str->add_column(sql::Column::create("group_id"));
    attrvals_str->add_column(sql::Column::create("value"));

    attrvals_real->add_column(sql::Column::create("attribute_id"));
    attrvals_real->add_column(sql::Column::create("group_id"));
    attrvals_real->add_column(sql::Column::create("value"));

    attrvals_bool->add_column(sql::Column::create("attribute_id"));
    attrvals_bool->add_column(sql::Column::create("group_id"));
    attrvals_bool->add_column(sql::Column::create("value"));

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
