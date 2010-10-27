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
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbNodeBackend.hpp>

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

long long
node_sql_type(const oh5::Node& node) {
    if (dynamic_cast<const oh5::Group*>(&node) != 0)
        return 1;
    else if (dynamic_cast<const oh5::Attribute*>(&node) != 0)
        return 2;
    else
        BRFC_ASSERT(false);
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



RdbHelper::RdbHelper(sql::Connection* conn, const FileHasher* hasher)
        : conn_(conn)
        , hasher_(hasher)
        , m_(Model::instance())
        , sql_() {

}

RdbHelper::~RdbHelper() {

}

const sql::Dialect&
RdbHelper::dialect() {
    return conn().dialect();    
}

sql::Connection&
RdbHelper::conn() {
    return *conn_;
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
        throw db_error("could not determine last inserted id");
    }
}

void
RdbHelper::insert_node(oh5::Node& node) {
    RdbFileEntry* file = dynamic_cast<RdbFileEntry*>(node.file());
    BRFC_ASSERT(file != 0);

    sql::InsertPtr qry = sql::Insert::create(m_.nodes);
    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry->add_return(m_.nodes->column("id"));

    const oh5::Node* parent = node.parent();
    if (parent) {
        long long parent_id = backend(*parent).id();
        qry->value("parent_id", sql_.int64_(parent_id));
    }

    qry->value("type", sql_.int64_(node_sql_type(node)));
    qry->value("file_id", sql_.int64_(file->id()));
    qry->value("name", sql_.string(node.name()));

    shared_ptr<sql::Result> result = conn().execute(*qry);

    long long db_id = last_id(*result);
    backend(node).id(db_id);

    if (oh5::Attribute* attr = dynamic_cast<oh5::Attribute*>(&node)) {
        insert_attribute(*attr);
    }

    backend(node).loaded(true);
}

void
RdbHelper::insert_attribute(oh5::Attribute& attr) {
    sql::InsertPtr qry = sql::Insert::create(m_.attrvals);

    qry->value("node_id", sql_.int64_(backend(attr).id()));
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

    const String& hash = hasher_->hash(file);

    long long source_id = select_source_id(file.source());

    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen();

    std::stringstream ss;
    ss << u;
    String uuid(ss.str());

    DateTime stored_at = DateTime::utc_now();
    stored_at.time().msec(0);
    
    qry->value("uuid", sql_.string(uuid));
    qry->value("hash", sql_.string(hash)); 
    qry->value("source_id", sql_.int64_(source_id));
    qry->value("stored_at", sql_.datetime(stored_at));
    qry->value("object", sql_.string(file.what_object()));
    qry->value("n_date", sql_.date(file.what_date()));
    qry->value("n_time", sql_.time(file.what_time()));

    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry->add_return(m_.files->column("id"));

    shared_ptr<sql::Result> result = conn().execute(*qry);

    long long file_id = last_id(*result);
    entry.uuid(uuid);
    entry.id(file_id);
    entry.source_id(source_id);
    entry.hash(hash);
    entry.stored_at(stored_at);
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
RdbHelper::load_file(RdbFileEntry& entry) {
    sql::SelectPtr qry = sql::Select::create();
    qry->from(m_.files->join(m_.file_content));
    qry->what(m_.files->column("uuid"));
    qry->what(m_.files->column("source_id"));
    qry->what(m_.files->column("hash"));
    qry->what(m_.files->column("stored_at"));
    qry->what(m_.file_content->column("lo_id"));
    qry->where(m_.files->column("id")->eq(sql_.int64_(entry.id())));
    
    shared_ptr<sql::Result> result = conn().execute(*qry);

    if (not result->next())
        throw brfc::lookup_error("no RdbFileEntry by id: "
                                 + String::number(entry.id()).to_std_string());
    entry.uuid(result->value_at("uuid").string());
    entry.source_id(result->value_at("source_id").int64_());
    entry.hash(result->value_at("hash").string());
    entry.lo_id(result->value_at("lo_id").int64_());
    entry.stored_at(result->value_at("stored_at").to_datetime());
}

long long
RdbHelper::select_root_id(const RdbFileEntry& entry) {
    sql::SelectPtr qry = sql::Select::create();
    qry->from(m_.nodes);
    qry->what(m_.nodes->column("id"));
    qry->where(
        sql_.and_(
            m_.nodes->column("file_id")->eq(sql_.int64_(entry.id())),
            m_.nodes->column("name")->eq(sql_.string(""))
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

    if (r->next())
        src.add("name", r->value_at("name").string());

    return src;
}

void
RdbHelper::load_children(oh5::Node& node) {
    sql::SelectPtr qry = sql::Select::create();
    
    qry->from(m_.nodes->outerjoin(m_.attrvals));
    qry->what(m_.nodes->column("id"));
    qry->what(m_.nodes->column("name"));
    qry->what(m_.nodes->column("type"));
    qry->what(m_.attrvals->column("value_int"));
    qry->what(m_.attrvals->column("value_str"));
    qry->what(m_.attrvals->column("value_real"));
    qry->what(m_.attrvals->column("value_bool"));
    qry->what(m_.attrvals->column("value_date"));
    qry->what(m_.attrvals->column("value_time"));

    long long id = backend(node).id();

    qry->where(m_.nodes->column("parent_id")->eq(sql_.int64_(id)));

    shared_ptr<sql::Result> r = conn().execute(*qry);

    while (r->next()) {
        const String& name = r->value_at("name").string();
        long long id = r->value_at("id").int64_();
        long long type = r->value_at("type").int64_();

        auto_ptr<oh5::Node> child;
        if (type == 1) { // GROUP
            child.reset(new oh5::Group(name));
        } else if (type == 2) { // ATTRIBUTE
            oh5::Scalar value(0);
            if (not r->value_at("value_str").is_null()) {
                value = oh5::Scalar(r->value_at("value_str").string());
            } else if (not r->value_at("value_int").is_null()) {
                value = oh5::Scalar(r->value_at("value_int").int64_());
            } else if (not r->value_at("value_real").is_null()) {
                value = oh5::Scalar(r->value_at("value_real").double_());
            } else {
                BRFC_ASSERT(false);
            }
            child.reset(new oh5::Attribute(name, value));
        } else {
            BRFC_ASSERT(false);
        }
        
        oh5::Node& c = backend(node).add_child(child.release());
        backend(c).id(id);
    }

    backend(node).loaded(true);
}

} // namespace rdb
} // namespace db
} // namespace brfc
