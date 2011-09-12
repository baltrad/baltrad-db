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

#include <brfc/rdb/RelationalDatabase.hpp>

#include <memory>

#include <boost/lexical_cast.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/FileHasher.hpp>
#include <brfc/SHA1AttributeHasher.hpp>
#include <brfc/Variant.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/rdb/QueryToSelect.hpp>
#include <brfc/rdb/RdbAttributeResult.hpp>
#include <brfc/rdb/RdbFileEntry.hpp>
#include <brfc/rdb/RdbFileResult.hpp>
#include <brfc/rdb/RdbQuery.hpp>
#include <brfc/rdb/RdbInDatabaseFileStorage.hpp>

#include <brfc/oh5/Oh5PhysicalFile.hpp>
#include <brfc/oh5/Oh5Node.hpp>
#include <brfc/oh5/Oh5Source.hpp>

#include <brfc/sql/BasicConnectionPool.hpp>
#include <brfc/sql/Connection.hpp>
#include <brfc/sql/DefaultConnectionCreator.hpp>
#include <brfc/sql/Result.hpp>
#include <brfc/sql/Select.hpp>

#include <brfc/util/uuid.hpp>
#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {

RelationalDatabase::RelationalDatabase(const Url& dsn)
        : creator_(new sql::DefaultConnectionCreator(dsn))
        , pool_(create_pool(creator_.get(), dsn))
        , storage_(new RdbInDatabaseFileStorage())
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher()) {
    init();
    conn(); // check if connection is valid
}

RelationalDatabase::RelationalDatabase(boost::shared_ptr<sql::ConnectionPool> pool)
        : creator_()
        , pool_(pool)
        , storage_(new RdbInDatabaseFileStorage())
        , mapper_(new AttributeMapper())
        , file_hasher_(new SHA1AttributeHasher()) {
    BRFC_ASSERT(pool);
    init();
}

void
RelationalDatabase::init() {
    populate_mapper();
    populate_hasher();
    storage_->database(this);
}

RelationalDatabase::~RelationalDatabase() {

}

sql::ConnectionPool*
RelationalDatabase::create_pool(sql::ConnectionCreator* conn_ctor,
                                const Url& dsn) {
    std::map<std::string, std::string> opts = dsn.http_searchpart();
    size_t max_size = 5;
    if (opts.find("pool_max_size") != opts.end()) {
        try {
            max_size = boost::lexical_cast<size_t>(opts["pool_max_size"]);
        } catch (const boost::bad_lexical_cast& e) {
            throw std::invalid_argument("invalid value for pool_max_size: "
                              + opts["pool_max_size"]);
        }
    }
    return new sql::BasicConnectionPool(conn_ctor, max_size);
}
                                

boost::shared_ptr<sql::Connection>
RelationalDatabase::conn() const {
    return boost::shared_ptr<sql::Connection>(pool_->get());
}

AttributeMapper&
RelationalDatabase::mapper() {
    return *mapper_;
}

const AttributeMapper&
RelationalDatabase::mapper() const {
    return *mapper_;
}

void
RelationalDatabase::storage_policy(RdbFileStoragePolicy* policy) {
    BRFC_ASSERT(policy);
    storage_.reset(policy);
}

bool
RelationalDatabase::do_is_stored(const Oh5PhysicalFile& file) {
    try {
        std::auto_ptr<FileEntry> e(entry_by_file(file));
    } catch (lookup_error) {
        return false;
    }
    return true;
}

FileEntry*
RelationalDatabase::do_store(const Oh5PhysicalFile& file) {
    std::auto_ptr<RdbFileEntry> entry(file_to_entry(file));

    {
        RdbQuery query(conn());
    
        const std::string& uuid =
            query.uuid_by_source_and_hash(entry->source_id(), entry->hash());
        if (not uuid.empty()) {
            throw duplicate_entry(file.path() + " already stored as " + uuid);
        }
    }

    storage_->store(*entry, file.path()); 
    return entry.release();
}

RdbFileEntry*
RelationalDatabase::file_to_entry(const Oh5PhysicalFile& file) {
    std::auto_ptr<RdbFileEntry> entry(new RdbFileEntry(this));

    entry->hash(file_hasher().hash(file));
    entry->loaded(true); // XXX: why?

    // copy nodes from file to entry
    Oh5NodeBackend& be = entry->root().backend();
    Oh5Node::const_iterator iter = file.root().begin();
    Oh5Node::const_iterator end = file.root().end();
    ++iter; // skip root;
    for ( ; iter != end; ++iter) {
        be.add(iter->parent()->path(), iter->clone());
    }

    // set attributes
    std::string uuid = uuid_string();
    entry->uuid(uuid);
    DateTime stored_at = DateTime::utc_now();
    entry->stored_at(stored_at);
    long long size = BoostFileSystem().file_size(file.path());
    entry->size(size);

    RdbQuery query(conn());
    long long source_id = query.select_source_id(file.source());
    entry->source_id(source_id);

    return entry.release();
}

void
RelationalDatabase::entry_to_file(const RdbFileEntry& entry,
                                  const std::string& path) {
    storage_->retrieve(entry, path);
}

FileEntry*
RelationalDatabase::do_entry_by_file(const Oh5PhysicalFile& file) {
    const std::string& hash = file_hasher().hash(file);
    RdbQuery query(conn());
    long long src_id = query.select_source_id(file.source());
    const std::string& uuid = query.uuid_by_source_and_hash(src_id, hash);

    if (uuid.empty())
        throw lookup_error(file.path() + " is not stored");
    
    return entry_by_uuid(uuid);
}

FileEntry*
RelationalDatabase::do_entry_by_uuid(const std::string& uuid) {
    std::auto_ptr<RdbFileEntry> entry(new RdbFileEntry(this));
    entry->uuid(uuid);
    entry->load();
    return entry.release();
}

FileResult*
RelationalDatabase::do_execute(const FileQuery& query) {
    const sql::Select& select = QueryToSelect(&mapper()).transform(query);
    boost::shared_ptr<sql::Connection> c = conn();
    std::auto_ptr<sql::Result> res(c->execute(select));
    return new RdbFileResult(this, c, res.release());
}

AttributeResult*
RelationalDatabase::do_execute(const AttributeQuery& query) {
    const sql::Select& select = QueryToSelect(&mapper()).transform(query);
    boost::shared_ptr<sql::Connection> c = conn();
    std::auto_ptr<sql::Result> res(c->execute(select));
    return new RdbAttributeResult(c, res.release());
}

void
RelationalDatabase::populate_mapper() {
    mapper_->clear();
    mapper_->add(Mapping("file:uuid", m::files::name(), "uuid"));
    mapper_->add(Mapping("file:hash", m::files::name(), "hash"));
    mapper_->add(Mapping("file:stored_at", m::files::name(), "stored_at"));
    mapper_->add(Mapping("file:size", m::files::name(),"size"));
    mapper_->add(Mapping("source:_name", m::sources::name(), "name"));
    mapper_->add(Mapping("what/object", m::files::name(), "what_object"));
    mapper_->add(Mapping("what/date", m::files::name(), "what_date"));
    mapper_->add(Mapping("what/time", m::files::name(), "what_time"));
}

void
RelationalDatabase::populate_hasher() {
    file_hasher_->clear_ignored();
    file_hasher_->ignore("what/source");
}

bool
RelationalDatabase::do_remove(const FileEntry& entry) {
    sql::Factory xpr;
    Expression stmt = Listcons().string("DELETE FROM bdb_files WHERE uuid = ")
                                .append(xpr.bind("uuid"))
                                .get();
    sql::Connection::BindMap_t binds;
    binds["uuid"] = Expression(entry.uuid());

    boost::shared_ptr<sql::Connection> c = conn();
    std::auto_ptr<sql::Result> r (c->execute(stmt, binds));
    return r->affected_rows();
}

std::vector<Oh5Source>
RelationalDatabase::do_sources() const {
    return RdbQuery(conn()).select_all_sources();
}

void
RelationalDatabase::do_add_source(const Oh5Source& source) {
    RdbQuery(conn()).add_source(source);
}

void
RelationalDatabase::do_update_source(const Oh5Source& source) {
    RdbQuery(conn()).update_source(source);
}

void
RelationalDatabase::do_remove_source(const Oh5Source& source) {
    RdbQuery(conn()).remove_source(source);
}

} // namespace brfc
