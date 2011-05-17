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

#include <brfc/db/rdb/RdbHelper.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>
#include <brfc/db/rdb/RdbFileEntry.hpp>
#include <brfc/db/rdb/RdbOh5NodeBackend.hpp>

#include <brfc/expr/Expression.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/PhysicalOh5File.hpp>

#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {

namespace {

oh5::Oh5Scalar
variant_to_oh5_scalar(const Variant& value) {
    switch (value.type()) {
        case Variant::STRING:
            return oh5::Oh5Scalar(value.string());
        case Variant::INT64:
            return oh5::Oh5Scalar(value.int64_());
        case Variant::DOUBLE:
            return oh5::Oh5Scalar(value.double_());
        case Variant::BOOL:
            return oh5::Oh5Scalar(value.bool_());
        case Variant::DATE:
            return oh5::Oh5Scalar(value.date());
        case Variant::TIME:
            return oh5::Oh5Scalar(value.time());
        default:
            break;
    }
    throw std::runtime_error("invalid Variant to oh5::Scalar");
}

Expression
attr_sql_value(const oh5::Oh5Attribute& attr) {
    switch (attr.value().type()) {
        case oh5::Oh5Scalar::STRING:
            return Expression(attr.value().string());
        case oh5::Oh5Scalar::INT64:
            return Expression(attr.value().int64_());
        case oh5::Oh5Scalar::DOUBLE:
            return Expression(attr.value().double_());
        default:
            break;
    }
    BRFC_ASSERT(false);
}

long long
node_sql_type(const oh5::Oh5Node& node) {
    if (dynamic_cast<const oh5::Oh5Group*>(&node) != 0)
        return 1;
    else if (dynamic_cast<const oh5::Oh5Attribute*>(&node) != 0)
        return 2;
    else if (dynamic_cast<const oh5::Oh5DataSet*>(&node) != 0)
        return 3;
    else
        BRFC_ASSERT(false);
}

std::string
attr_sql_column(const oh5::Oh5Attribute& attr) {
    switch (attr.value().type()) {
        case oh5::Oh5Scalar::STRING:
            return "value_str";
        case oh5::Oh5Scalar::INT64:
            return "value_int";
        case oh5::Oh5Scalar::DOUBLE:
            return "value_double";
        default:
            break;
    }
    BRFC_ASSERT(false);
}

} // namespace anonymous



RdbHelper::RdbHelper(shared_ptr<sql::Connection> conn)
        : conn_(conn)
        , sql_()
        , insert_node_qry_()
        , insert_attr_qry_() {
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

RdbOh5NodeBackend&
RdbHelper::backend(oh5::Oh5Node& node) const {
    return dynamic_cast<RdbOh5NodeBackend&>(node.backend());
}

const RdbOh5NodeBackend&
RdbHelper::backend(const oh5::Oh5Node& node) const {
    return dynamic_cast<const RdbOh5NodeBackend&>(node.backend());
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
RdbHelper::compile_insert_node_query() {
    sql::Insert qry(m::nodes::name());
    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry.returning(m::nodes::column("id"));

    qry.value("parent_id", sql_.bind("parent_id"));
    qry.value("type", sql_.bind("type"));
    qry.value("file_id", sql_.bind("file_id"));
    qry.value("name", sql_.bind("name"));

    insert_node_qry_ = conn().compiler().compile(qry.expression());
}

void
RdbHelper::insert_node(long long file_id, oh5::Oh5Node& node) {
    if (not insert_node_qry_)
        compile_insert_node_query();
    
    sql::Connection::BindMap_t binds;
    Expression parent_id;
    const oh5::Oh5Node* parent = node.parent();
    if (parent) {
        parent_id = Expression(backend(*parent).id(*parent));
    }
    binds["parent_id"] = parent_id;
    binds["type"] = Expression(node_sql_type(node));
    binds["file_id"] = Expression(file_id);
    binds["name"] = Expression(node.name());

    scoped_ptr<sql::Result> result(conn().execute(insert_node_qry_, binds));

    long long db_id = last_id(*result);
    backend(node).id(node, db_id);

    if (oh5::Oh5Attribute* attr = dynamic_cast<oh5::Oh5Attribute*>(&node)) {
        insert_attribute(*attr);
    }

    backend(node).loaded(node, true);
}

void
RdbHelper::compile_insert_attr_query() {
    sql::Insert qry(m::attrvals::name());

    qry.value("node_id", sql_.bind("node_id"));
    qry.value("value_str", sql_.bind("value_str"));
    qry.value("value_int", sql_.bind("value_int"));
    qry.value("value_double", sql_.bind("value_double"));
    qry.value("value_bool", sql_.bind("value_bool"));
    qry.value("value_date", sql_.bind("value_date"));
    qry.value("value_time", sql_.bind("value_time"));
    
    insert_attr_qry_ = conn().compiler().compile(qry.expression());
}

void
RdbHelper::insert_attribute(oh5::Oh5Attribute& attr) {
    if (not insert_attr_qry_)
        compile_insert_attr_query();
    
    sql::Connection::BindMap_t binds;
    binds["node_id"] = Expression(backend(attr).id(attr));
    binds[attr_sql_column(attr)] = attr_sql_value(attr);
    if (attr.value().type() == oh5::Oh5Scalar::STRING) {
        try {
            bool val = attr.value().to_bool();
            binds["value_bool"] = Expression(val);
        } catch (const value_error&) { /* pass */ }

        try {
            Date date = attr.value().to_date();
            binds["value_date"] = Expression(date);
        } catch (const value_error&) { /* pass */ }

        try {
            Time time = attr.value().to_time();
            binds["value_time"] = Expression(time);
        } catch (const value_error&) { /* pass */ }
    }

    scoped_ptr<sql::Result>(conn().execute(insert_attr_qry_, binds));
}

void
RdbHelper::insert_file(RdbFileEntry& entry,
                       const oh5::PhysicalOh5File& file) {
    long long source_id = select_source_id(file.source());

    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen();

    std::stringstream ss;
    ss << u;
    std::string uuid(ss.str());

    DateTime stored_at = DateTime::utc_now();
    
    sql::Insert qry(m::files::name());
    qry.value("uuid", sql_.string(uuid));
    qry.value("hash", sql_.string(entry.hash())); 
    qry.value("source_id", sql_.int64_(source_id));
    qry.value("stored_at", sql_.datetime(stored_at));
    qry.value("what_object", sql_.string(file.what_object()));
    qry.value("what_date", sql_.date(file.what_date()));
    qry.value("what_time", sql_.time(file.what_time()));

    if (dialect().has_feature(sql::Dialect::RETURNING))
        qry.returning(m::files::column("id"));

    scoped_ptr<sql::Result> result(conn().execute(qry));

    long long file_id = last_id(*result);
    entry.uuid(uuid);
    entry.id(file_id);
    entry.source_id(source_id);
    entry.stored_at(stored_at);
    entry.loaded(true);
}

void
RdbHelper::insert_file_content(RdbFileEntry& entry, const std::string& path) {
    // transfer the file to database
    long long size = BoostFileSystem().file_size(path);
    long long lo_id = conn().store_large_object(path);

    sql::Insert qry(m::file_content::name());
    qry.value("file_id", sql_.int64_(entry.id()));
    qry.value("lo_id", sql_.int64_(lo_id));
    qry.value("size", sql_.int64_(size));

    scoped_ptr<sql::Result>(conn().execute(qry));

    entry.lo_id(lo_id);
    entry.size(size);
}

void
RdbHelper::load_file(RdbFileEntry& entry) {
    sql::Select qry;
    qry.from(sql_.table(m::files::name()));
    qry.outerjoin(
        sql_.table(m::file_content::name()),
        sql_.eq(m::files::column("id"), m::file_content::column("file_id"))
    );

    qry.what(m::files::column("uuid"));
    qry.what(m::files::column("source_id"));
    qry.what(m::files::column("hash"));
    qry.what(m::files::column("stored_at"));
    qry.what(m::file_content::column("lo_id"));
    qry.what(m::file_content::column("size"));
    if (entry.id() != 0)
        qry.where(sql_.eq(m::files::column("id"), sql_.int64_(entry.id())));
    else if (entry.uuid() != "")
        qry.where(sql_.eq(m::files::column("uuid"), sql_.string(entry.uuid())));
    else
        throw std::runtime_error("no uuid or id to load file from db");
    
    scoped_ptr<sql::Result> result(conn().execute(qry));

    if (not result->next())
        throw brfc::lookup_error("no RdbFileEntry found by id=" +
                                 boost::lexical_cast<std::string>(entry.id()) +
                                 " uuid=" + entry.uuid());

    entry.uuid(result->value_at("uuid").string());
    entry.source_id(result->value_at("source_id").int64_());
    entry.hash(result->value_at("hash").string());
    entry.lo_id(result->value_at("lo_id").to_int64());
    entry.size(result->value_at("size").to_int64());
    entry.stored_at(result->value_at("stored_at").to_datetime());
}

long long
RdbHelper::select_root_id(const RdbFileEntry& entry) {
    sql::Select qry;
    qry.from(sql_.table(m::files::name()));
    qry.join(
        sql_.table(m::nodes::name()),
        sql_.eq(m::files::column("id"), m::nodes::column("file_id"))
    );
    qry.what(m::nodes::column("id"));
    qry.where(sql_.eq(m::nodes::column("name"), sql_.string("")));
    if (entry.id() != 0)
        qry.append_where(sql_.eq(m::files::column("id"), sql_.int64_(entry.id())));
    else if (entry.uuid() != "")
        qry.append_where(sql_.eq(m::files::column("uuid"), sql_.string(entry.uuid())));
    else
        throw std::runtime_error("no uuid or id to load file from db");

    scoped_ptr<sql::Result> r(conn().execute(qry));
    
    if (not r->next())
        return 0;

    return r->value_at(0).int64_();
}

long long
RdbHelper::select_source_id(const oh5::Oh5Source& src) {
    sql::Select qry;
    qry.distinct(true);
    qry.what(m::sources::column("id"));
    qry.from(sql_.table(m::sources::name()));
    qry.join(
        sql_.table(m::source_kvs::name()),
        sql_.eq(
            m::sources::column("id"),
            m::source_kvs::column("source_id")
        )
    );
    qry.what(m::sources::column("id"));

    qry.where(sql_.bool_(false));
    Expression x;
    BOOST_FOREACH(const std::string& key, src.keys()) {
        x = sql_.and_(
            sql_.eq(m::source_kvs::column("key"), sql_.string(key)),
            sql_.eq(m::source_kvs::column("value"), sql_.string(src.get(key)))
        );
        qry.where(sql_.or_(qry.where(), x));
    }

    scoped_ptr<sql::Result> r(conn().execute(qry));
    if (r->size() == 1 and r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

oh5::Oh5Source
RdbHelper::select_source(long long id) {
    sql::Select qry;
    qry.what(m::source_kvs::column("key"));
    qry.what(m::source_kvs::column("value"));
    qry.from(sql_.table(m::source_kvs::name()));
    qry.where(sql_.eq(m::source_kvs::column("source_id"), sql_.int64_(id)));

    scoped_ptr<sql::Result> r(conn().execute(qry));

    oh5::Oh5Source src;
    
    while (r->next()) {
        src.add(r->value_at("key").string(), r->value_at("value").string());
    }

    qry = sql::Select();
    qry.what(m::sources::column("id"));
    qry.what(m::sources::column("name"));
    qry.from(sql_.table(m::sources::name()));
    qry.where(sql_.eq(m::sources::column("id"), sql_.int64_(id)));

    r.reset(conn().execute(qry));

    if (r->next()) {
        src.add("_name", r->value_at("name").string());
        src.add("_id", r->value_at("id").to_string());
    }

    return src;
}

std::vector<oh5::Oh5Source>
RdbHelper::select_all_sources() {
    sql::Select qry;
    qry.from(sql_.table(m::sources::name()));
    qry.outerjoin(
        sql_.table(m::source_kvs::name()),
        sql_.eq(
            m::sources::column("id"),
            m::source_kvs::column("source_id")
        )
    );

    qry.what(m::sources::column("id"));
    qry.what(m::sources::column("name"));
    qry.what(m::source_kvs::column("key"));
    qry.what(m::source_kvs::column("value"));

    scoped_ptr<sql::Result> r(conn().execute(qry));

    oh5::Oh5Source src;
    long long prev_id = 0, id = 0;
    std::vector<oh5::Oh5Source> sources;
    while (r->next()) {
        id = r->value_at("id").int64_();
        if (id != prev_id) {
            if (prev_id != 0) {
                sources.push_back(src);
                src.clear();
            }
            prev_id = id;
            src.add("_name", r->value_at("name").string());
            src.add("_id", boost::lexical_cast<std::string>(id));
        }
        if (r->value_at("key").is_null())
            continue; // no key:value pairs associated
        src.add(r->value_at("key").string(), r->value_at("value").string());
    }
    sources.push_back(src);
    return sources;
}

void
RdbHelper::add_source(const oh5::Oh5Source& source) {
    scoped_ptr<sql::Result> r;
    try {
        conn().begin();
        sql::Insert qry(m::sources::name());
        qry.value("name", sql_.string(source.get("_name")));
        if (dialect().has_feature(sql::Dialect::RETURNING))
            qry.returning(m::sources::column("id"));
        r.reset(conn().execute(qry));
        long long id = last_id(*r);

        BOOST_FOREACH(const std::string& key, source.keys()) {
            qry = sql::Insert(m::source_kvs::name());
            qry.value("source_id", sql_.int64_(id));
            qry.value("key", sql_.string(key));
            qry.value("value", sql_.string(source.get(key)));
            r.reset(conn().execute(qry));
        }

        conn().commit();
    } catch (...) {
        conn().rollback();
        throw;
    }
}

void
RdbHelper::update_source(const oh5::Oh5Source& source) {
    long long id = boost::lexical_cast<long long>(source.get("_id"));
    scoped_ptr<sql::Result> r;
    try {
        conn().begin();

        Expression stmt = Listcons().string("UPDATE bdb_sources SET name = ")
                                    .append(sql_.bind("name"))
                                    .string(" WHERE id = ")
                                    .append(sql_.bind("id"))
                                    .get();
        sql::Connection::BindMap_t binds;
        binds["name"] = Expression(source.get("_name"));
        binds["id"] = Expression(id);
        r.reset(conn().execute(stmt, binds));

        stmt = Listcons().string("DELETE FROM bdb_source_kvs WHERE source_id =")
                         .append(sql_.bind("id"))
                         .get();
        binds.clear();
        binds["id"] = Expression(id);
        r.reset(conn().execute(stmt, binds));
    
        BOOST_FOREACH(const std::string& key, source.keys()) {
            sql::Insert qry(m::source_kvs::name());
            qry.value("source_id", sql_.int64_(id));
            qry.value("key", sql_.string(key));
            qry.value("value", sql_.string(source.get(key)));
            r.reset(conn().execute(qry));
        }

        conn().commit();
    } catch (...) {
        conn().rollback();
        throw;
    }
}

void
RdbHelper::remove_source(const oh5::Oh5Source& source) {
    Expression stmt = Listcons().string("DELETE FROM bdb_sources WHERE id=")
                                .append(sql_.bind("id"))
                                .get();
    sql::Connection::BindMap_t binds;
    binds["id"] = Expression(boost::lexical_cast<int>(source.get("_id")));
    scoped_ptr<sql::Result> r(conn().execute(stmt, binds));
    if (not r->affected_rows())
        throw lookup_error("source not stored in database");
}

void
RdbHelper::load_children(oh5::Oh5Node& node) {
    sql::Select qry;
    qry.from(sql_.table(m::nodes::name()));
    qry.outerjoin(
        sql_.table(m::attrvals::name()),
        sql_.eq(
            m::nodes::column("id"),
            m::attrvals::column("node_id")
        )
    );

    qry.what(m::nodes::column("id"));
    qry.what(m::nodes::column("name"));
    qry.what(m::nodes::column("type"));
    qry.what(m::attrvals::column("value_int"));
    qry.what(m::attrvals::column("value_str"));
    qry.what(m::attrvals::column("value_double"));
    qry.what(m::attrvals::column("value_bool"));
    qry.what(m::attrvals::column("value_date"));
    qry.what(m::attrvals::column("value_time"));

    RdbOh5NodeBackend& be = backend(node);

    long long id = be.id(node);

    qry.where(sql_.eq(m::nodes::column("parent_id"), sql_.int64_(id)));

    scoped_ptr<sql::Result> r(conn().execute(qry));

    while (r->next()) {
        std::string name = r->value_at("name").string();
        long long id = r->value_at("id").int64_();
        long long type = r->value_at("type").int64_();

        auto_ptr<oh5::Oh5Node> child;
        if (type == 1) { // GROUP
            child.reset(new oh5::Oh5Group(name));
        } else if (type == 2) { // ATTRIBUTE
            oh5::Oh5Scalar value(0);
            if (not r->value_at("value_str").is_null()) {
                value = oh5::Oh5Scalar(r->value_at("value_str").string());
            } else if (not r->value_at("value_int").is_null()) {
                value = oh5::Oh5Scalar(r->value_at("value_int").int64_());
            } else if (not r->value_at("value_double").is_null()) {
                value = oh5::Oh5Scalar(r->value_at("value_double").double_());
            } else {
                BRFC_ASSERT(false);
            }
            child.reset(new oh5::Oh5Attribute(name, value));
        } else if (type == 3) { // DATASET
            child.reset(new oh5::Oh5DataSet(name));
        } else {
            BRFC_ASSERT(false);
        }
        
        oh5::Oh5Node& c = be.add(node, child.release());
        be.id(c, id);
        be.loaded(c, false);
    }

    be.loaded(node, true);
}

} // namespace brfc
