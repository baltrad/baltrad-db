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

#include <brfc/rdb/attr_to_column.hpp>

#include <boost/assign.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/assert.hpp>
#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>

namespace brfc {

attr_to_column::attr_to_column(const AttributeMapper* mapper)
        : mapper_(mapper)
        , xpr_()
        , from_()
        , plain_column_map() {
    boost::assign::insert(plain_column_map)
        ("string", "value_str")
        ("int64", "value_int")
        ("double", "value_double")
        ("bool", "value_bool")
        ("date", "value_date")
        ("time", "value_time")
    ;
    reset();
}

void
attr_to_column::reset() {
    // always select from files and join sources
    from_ = sql::FromClause(xpr_.table(m::files::name()));
}

void
attr_to_column::join_sources() {
    Expression sources_t = xpr_.table(m::sources::name());
    if (not from_.contains(sources_t)) {
        from_.join(
            sources_t,
            xpr_.eq(
                m::files::column("source_id"),
                m::sources::column("id")
            )
        );
    }
}

std::string
attr_to_column::join_source_kvs(const std::string& key) {
    std::string alias = "src_" + key;
    Expression alias_t = xpr_.alias(xpr_.table("bdb_source_kvs"), alias);

    // join if missing
    if (not from_.contains(alias_t)) {
        from_.outerjoin(
            alias_t,
            xpr_.and_(
                xpr_.eq(
                    xpr_.column(alias, "source_id"),
                    m::sources::column("id")
                ),
                xpr_.eq(
                    xpr_.column(alias, "key"),
                    xpr_.string(key)
                )
            )
        );
    }
    return alias;
}

Expression
attr_to_column::operator()(const Expression& x) {
    BRFC_ASSERT(x.is_list());
    BRFC_ASSERT(x.size() == 2);

    Expression::const_iterator it = x.begin();
    const std::string& name = (*it).string();
    ++it;
    const std::string& type = (*it).string();
    return column_by_attr(name, type);
}

Expression
attr_to_column::column_by_attr(const std::string& name,
                               const std::string& type) {
    if (boost::starts_with(name, "what/source:")) {
        return source_attr_column(name);
    } else if (mapper_->has(name)) {
        return specialized_attr_column(name);
    } else {
        return plain_attr_column(name, type);
    }
}

/**
 * @brief look up sql::Column for a specialized attribute
 * @return the sql::Column where the attribute is stored
 *
 * look up mapping from the mapper and use Mapping::column as the column
 * for the attribute.
 */
Expression
attr_to_column::specialized_attr_column(const std::string& name) {
    // currently, specialized attributes are only in bdb_files and
    // bdb_sources. These are already contained in the from clause.
    Mapping mapping = mapper_->mapping(name);
    return xpr_.column(mapping.table, mapping.column);
}

/**
 * @brief look up sql::Column for a source attribute
 * @return the sql::Column where the attribute is stored
 *
 * The column is looked up using the $KEY in 'what/source:$KEY':
 * - if $KEY is '_name', the column for the attribute is 'bdb_sources.name'
 * - for every other $KEY, 'bdb_source_kvs' is joined in the from-clause
 *   (if not already joined) using an alias 'src_$KEY' with an additional
 *   join-condition 'bdb_source_kvs.key=$KEY'. The column for the
 *   attribute is 'src_$KEY.value'.
 */
Expression
attr_to_column::source_attr_column(const std::string& name) {
    const std::string& key = name.substr(name.find_first_of(':') + 1);
    if (key == "_name") {
        join_sources();
        // this attribute can be accessed at sources.name
        return m::sources::column("name");
    } else {
        join_sources();
        const std::string& alias = join_source_kvs(key);
        // this attribute can be accessed at src_$KEY.value
        return xpr_.column(alias, "value");
    }
}

/**
 * @brief look up sql::Column for a plain attribute
 * @return the sql::Column where the attribute is stored
 *
 * Call join_attrs() to join 'attrs' and 'pnode1'. Alias
 * 'bdb_attribute_values' for this attribute as '$NAME_values' and join
 * it in the from clause (if not already joined).
 *
 * The column for the attribute is '$NAME_values.value_$TYPE'.
 */
Expression
attr_to_column::plain_attr_column(std::string name, const std::string& type) {
    std::list<std::string> path;
    boost::split(path, name, boost::is_any_of("/"),
                 boost::token_compress_on);
    std::string attrname = path.back();
    path.pop_back();
    std::string groupname;
    if (not path.empty()) {
        groupname = path.back();
        path.pop_back();
    }
    
    // alias the table (this attribute is always searched on this alias)
    boost::erase_all(name, "/");
    std::string alias = name + "_values";
    Expression alias_t = xpr_.alias(xpr_.table(m::attrvals::name()), alias);
    
    // join this table-alias if not already joined
    if (not from_.contains(alias_t)) {
        // join attribute layer to files
        std::string l0_alias = name + "_l0";
        from_.outerjoin(
            xpr_.alias(xpr_.table(m::nodes::name()), l0_alias),
            xpr_.and_(
                xpr_.eq(
                    xpr_.column(l0_alias, "file_id"),
                    m::files::column("id")
                ),
                xpr_.eq(
                    xpr_.column(l0_alias, "name"),
                    xpr_.string(attrname)
                )
            )
        );
        // join group layer to attribute layer
        std::string l1_alias = name + "_l1";
        from_.outerjoin(
            xpr_.alias(xpr_.table(m::nodes::name()), l1_alias),
            xpr_.and_(
                xpr_.eq(
                    xpr_.column(l1_alias, "id"),
                    xpr_.column(l0_alias, "parent_id")
                ),
                xpr_.eq(
                    xpr_.column(l1_alias, "name"),
                    xpr_.string(groupname)
                )
            )
        );
        // join attribute values to attribute layer
        from_.outerjoin(
            alias_t,
            xpr_.eq(
                xpr_.column(alias, "node_id"),
                xpr_.column(l0_alias, "id")
            )
        );
    }
    return xpr_.column(alias, plain_column_map[type]);
}

} // namespace brfc
