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

#include <brfc/rdb/RdbDefaultSourceManager.hpp>

#include <memory>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>
#include <brfc/oh5/Oh5File.hpp>

#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Select.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>

#include <brfc/util/uuid.hpp>

namespace brfc {

RdbDefaultSourceManager::RdbDefaultSourceManager(boost::shared_ptr<sql::Connection> conn)
        : conn_(conn)
        , sql_() {
}

RdbDefaultSourceManager::~RdbDefaultSourceManager() {

}

const sql::Dialect&
RdbDefaultSourceManager::dialect() {
    return conn().dialect();    
}

sql::Connection&
RdbDefaultSourceManager::conn() {
    return *conn_;
}

long long
RdbDefaultSourceManager::last_id(sql::Result& result) {
    if (dialect().has_feature(sql::Dialect::RETURNING) and result.next()) {
        return result.value_at(0).int64_();
    } else if (dialect().has_feature(sql::Dialect::LAST_INSERT_ID)) {
        return conn().last_insert_id();
    } else {
        throw db_error("could not determine last inserted id");
    }
}

long long
RdbDefaultSourceManager::do_source_id(const Oh5Source& src) {
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

    boost::scoped_ptr<sql::Result> r(conn().execute(qry));
    if (r->size() == 1 and r->next())
        return r->value_at(0).int64_();
    else
        return 0;
}

Oh5Source
RdbDefaultSourceManager::do_source_by_id(long long id) {
    sql::Select qry;
    qry.what(m::source_kvs::column("key"));
    qry.what(m::source_kvs::column("value"));
    qry.from(sql_.table(m::source_kvs::name()));
    qry.where(sql_.eq(m::source_kvs::column("source_id"), sql_.int64_(id)));

    boost::scoped_ptr<sql::Result> r(conn().execute(qry));

    Oh5Source src;
    
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

std::vector<Oh5Source>
RdbDefaultSourceManager::do_all_sources() {
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

    boost::scoped_ptr<sql::Result> r(conn().execute(qry));

    Oh5Source src;
    long long prev_id = 0, id = 0;
    std::vector<Oh5Source> sources;
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
RdbDefaultSourceManager::do_add_source(const Oh5Source& source) {
    boost::scoped_ptr<sql::Result> r;
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
RdbDefaultSourceManager::do_update_source(const Oh5Source& source) {
    long long id = boost::lexical_cast<long long>(source.get("_id"));
    boost::scoped_ptr<sql::Result> r;
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
RdbDefaultSourceManager::do_remove_source(const Oh5Source& source) {
    Expression stmt = Listcons().string("DELETE FROM bdb_sources WHERE id=")
                                .append(sql_.bind("id"))
                                .get();
    sql::Connection::BindMap_t binds;
    binds["id"] = Expression(boost::lexical_cast<int>(source.get("_id")));
    boost::scoped_ptr<sql::Result> r(conn().execute(stmt, binds));
    if (not r->affected_rows())
        throw lookup_error("source not stored in database");
}

} // namespace brfc
