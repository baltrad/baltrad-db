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

#include <brfc/db/rdb/RdbHelper.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>
#include <brfc/db/rdb/RelationalDatabase.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Group.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/oh5/PhysicalFile.hpp>

#include <brfc/sql/expr.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/LargeObject.hpp>
#include <brfc/sql/Result.hpp>

namespace brfc {
namespace db {
namespace rdb {

namespace {

oh5::Scalar
variant_to_oh5_scalar(const Variant& value) {
    switch (value.type()) {
        case Variant::STRING:
            return oh5::Scalar(value.string());
        case Variant::INT64:
            return oh5::Scalar(value.int64_());
        case Variant::DOUBLE:
            return oh5::Scalar(value.double_());
        case Variant::BOOL:
            return oh5::Scalar(value.bool_());
        case Variant::DATE:
            return oh5::Scalar(value.date());
        case Variant::TIME:
            return oh5::Scalar(value.time());
        default:
            throw std::runtime_error("invalid Variant to oh5::Scalar");
    }
}

sql::LiteralPtr
attr_sql_value(const oh5::Attribute& attr) {
    switch (attr.value().type()) {
        case oh5::Scalar::STRING:
            return sql::Literal::create(Variant(attr.value().string()));
        case oh5::Scalar::INT64:
            return sql::Literal::create(Variant(attr.value().int64_()));
        case oh5::Scalar::DOUBLE:
            return sql::Literal::create(Variant(attr.value().double_()));
        default:
            BRFC_ASSERT(false);
    }
}

sql::ColumnPtr
attr_sql_column(const oh5::Attribute& attr) {
    const Model& m = Model::instance();
    switch (attr.value().type()) {
        case oh5::Scalar::STRING:
            return m.attrvals->column("value_str");
        case oh5::Scalar::INT64:
            return m.attrvals->column("value_int");
        case oh5::Scalar::DOUBLE:
            return m.attrvals->column("value_real");
        default:
            BRFC_ASSERT(false);
    }
}

} // namespace anonymous



RdbHelper::RdbHelper(RelationalDatabase* rdb)
        : rdb_(rdb)
        , m_(Model::instance())
        , sql_() {

}

RdbHelper::~RdbHelper() {

}

const sql::Dialect&
RdbHelper::dialect() {
    return rdb().conn().dialect();    
}

sql::Connection&
RdbHelper::conn() {
    return rdb().conn();
}

RdbNodeBackend&
RdbHelper::backend(oh5::Node& node) const {
    return dynamic_cast<RdbNodeBackend&>(node.backend());
}

const RdbNodeBackend&
RdbHelper::backend(const oh5::Node& node) const {
    return dynamic_cast<const RdbNodeBackend&>(node.backend());
}

long long
RdbHelper::last_id(sql::Result& result) {
    if (dialect().has_feature(sql::Dialect::RETURNING) and result.next()) {
        return result.value_at(0).int64_();
    } else if (dialect().has_feature(sql::Dialect::LAST_INSERT_ID)) {
        return conn().last_insert_id();
    } else {
        throw db_error("could not determine inserted group id");
    }
}

void
RdbHelper::insert_node(oh5::Node& node) {
    if (oh5::Group* grp = dynamic_cast<oh5::Group*>(&node)) {
        insert_group(*grp);
    } else if (oh5::Attribute* attr = dynamic_cast<oh5::Attribute*>(&node)) {
        insert_attribute(*attr);
    } else {
        throw std::runtime_error("could not determine insert");
    }
}

void
RdbHelper::insert_attribute(oh5::Attribute& attr) {
    // XXX: checking for valid mapping should be eliminated
    //
    // this requires saving attributes as nodes in bdb_groups (to store
    // the name, and dropping reference to bdb_attributes from
    // bdb_attribute_values
    //
    // this would also change how attributes are queried for
    if (not rdb().mapper().has(attr.full_name()))
        return;

    const Mapping& mapping = rdb().mapper().mapping(attr.full_name());
    
    sql::InsertPtr qry = sql::Insert::create(m_.attrvals);

    const oh5::Group* parent = attr.parent<oh5::Group>();
    if (parent) {
        long long parent_id = backend(*parent).id();
        qry->value("group_id", sql_.int64_(parent_id));
    }

    qry->value("attribute_id", sql_.int64_(mapping.id));
    qry->value(attr_sql_column(attr)->name(), attr_sql_value(attr));

    if (attr.value().type() == oh5::Scalar::STRING) {
        try {
            bool val = attr.value().to_bool();
            qry->value("value_bool", sql_.bool_(val));
        } catch (const value_error&) { /* pass */ }

        try {
            Date date = attr.value().to_date();
            qry->value("value_date", sql_.date(date));
        } catch (const value_error&) { /* pass */ }

        try {
            Time time = attr.value().to_time();
            qry->value("value_time", sql_.time(time));
        } catch (const value_error&) { /* pass */ }
    }

    conn().execute(*qry);
}

void
RdbHelper::insert_file(RdbFileEntry& entry,
                       const oh5::PhysicalFile& file) {
    sql::InsertPtr qry = sql::Insert::create(m_.files);

    const String& hash = rdb().file_hasher().hash(file);

    long long source_id = select_source_id(file.source());
 
    qry->value("hash_type", sql_.string(rdb().file_hasher().name()));
    qry->value("unique_id", sql_.string(hash)); 
    qry->value("source_id", sql_.int64_(source_id));
    qry->value("object", sql_.string(file.what_object()));
    qry->value("n_date", sql_.date(file.what_date()));
    qry->value("n_time", sql_.time(file.what_time()));

    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry->add_return(m_.files->column("id"));

    shared_ptr<sql::Result> result = conn().execute(*qry);

    long long file_id = last_id(*result);
    entry.id(file_id);
    entry.source_id(source_id);
}

void
RdbHelper::insert_file_content(RdbFileEntry& entry, const String& path) {
    // transfer the file to database
    shared_ptr<sql::LargeObject> lo = conn().large_object(path);

    sql::InsertPtr qry = sql::Insert::create(m_.file_content);
    qry->value("file_id", sql_.int64_(entry.id()));
    qry->value("lo_id", sql_.int64_(lo->id()));

    conn().execute(*qry);

    entry.lo_id(lo->id());
}



void
RdbHelper::insert_group(oh5::Group& group) {
    RdbFileEntry* file = dynamic_cast<RdbFileEntry*>(group.file());
    BRFC_ASSERT(file != 0);

    sql::InsertPtr qry = sql::Insert::create(m_.groups);
    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry->add_return(m_.groups->column("id"));

    const oh5::Group* parent = group.parent<oh5::Group>();
    if (parent) {
        long long parent_id = backend(*parent).id();
        qry->value("parent_id", sql_.int64_(parent_id));
    }

    qry->value("file_id", sql_.int64_(file->id()));
    qry->value("name", sql_.string(group.name()));

    shared_ptr<sql::Result> result = conn().execute(*qry);

    long long db_id = last_id(*result);
    backend(group).id(db_id);
}

void
RdbHelper::load_file(RdbFileEntry& entry) {

}

long long
RdbHelper::select_root_id(const RdbFileEntry& entry) {
    sql::SelectPtr qry = sql::Select::create();
    qry->from(m_.groups);
    qry->what(m_.groups->column("id"));
    qry->where(
        sql_.and_(
            m_.groups->column("file_id")->eq(sql_.int64_(entry.id())),
            m_.groups->column("name")->eq(sql_.string(""))
        )
    );

    shared_ptr<sql::Result> r = conn().execute(*qry);
    
    r->next();
    return r->value_at(0).int64_();
}

long long
RdbHelper::select_source_id(const oh5::Source& src) {
    sql::SelectPtr qry = sql::Select::create();
    qry->distinct(true);
    qry->what(m_.sources->column("id"));
    qry->from(m_.sources->join(m_.source_kvs));
    qry->where(sql_.bool_(false));
    
    sql::ExpressionPtr x;
    BOOST_FOREACH(const String& key, src.keys()) {
        x = m_.source_kvs->column("key")->eq(sql_.string(key));
        x = x->and_(m_.source_kvs->column("value")->eq(sql_.string(src.at(key))));
        qry->where(qry->where()->or_(x->parentheses()));
    }

    shared_ptr<sql::Result> r = conn().execute(*qry);
    if (r->size() == 1 and r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

oh5::Source
RdbHelper::select_source(long long id) {
    sql::SelectPtr qry = sql::Select::create(m_.source_kvs);
    qry->where(m_.source_kvs->column("source_id")->eq(sql_.int64_(id)));

    shared_ptr<sql::Result> r = conn().execute(*qry);

    oh5::Source src;
    
    while (r->next()) {
        src.add(r->value_at("key").string(), r->value_at("value").string());
    }

    qry = sql::Select::create(m_.sources);
    qry->where(m_.sources->column("id")->eq(sql_.int64_(id)));

    r = conn().execute(*qry);

    r->next();
    src.add("name", r->value_at("name").string());

    return src;
}

void
RdbHelper::load_children(oh5::Node& node) {
    // query child groups
    sql::SelectPtr qry = sql::Select::create();
    
    qry->from(m_.groups);
    qry->what(m_.groups->column("id"));
    qry->what(m_.groups->column("name"));

    long long id = backend(node).id();

    qry->where(m_.groups->column("parent_id")->eq(sql_.int64_(id)));

    shared_ptr<sql::Result> r = conn().execute(*qry);

    while (r->next()) {
        const String& name = r->value_at("name").string();
        long long id = r->value_at("id").int64_();
        oh5::Group& grp = node.create_group(name);
        RdbNodeBackend& backend = static_cast<RdbNodeBackend&>(grp.backend());
        backend.id(id);
        backend.loaded(false);
    }
    
    // query child attributes
    qry = sql::Select::create();
    qry->from(m_.attrvals->join(m_.attrs));
    qry->what(m_.attrs->column("name"));
    qry->what(m_.attrs->column("storage_column"));
    qry->what(m_.attrvals->column("value_int"));
    qry->what(m_.attrvals->column("value_str"));
    qry->what(m_.attrvals->column("value_real"));
    qry->what(m_.attrvals->column("value_bool"));
    qry->what(m_.attrvals->column("value_date"));
    qry->what(m_.attrvals->column("value_time"));
    qry->where(m_.attrvals->column("group_id")->eq(sql_.int64_(id)));
    r = conn().execute(*qry);

    while (r->next()) {
        const String& name = r->value_at("name").string();
        const String& value_col = r->value_at("storage_column").string();
        const Variant& value = r->value_at(value_col);
        node.create_attribute(name, variant_to_oh5_scalar(value));
    }
}

} // namespace rdb
} // namespace db
} // namespace brfc
