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

#include <brfc/test/TestRDB.hpp>

#include <list>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Url.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Result.hpp>

#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {
namespace test {

TestRDB::TestRDB(const std::string& dsn, const std::string& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(schema_dir) {
    if (not BoostFileSystem().is_absolute(schema_dir_))
        throw value_error("schema dir must be a complete path");
}

TestRDB::~TestRDB() {

}

void
TestRDB::create() {
    exec_queries_from("create.sql");
}

void
TestRDB::drop() {
    exec_queries_from("drop.sql");
}

void
TestRDB::clean() {
    scoped_ptr<sql::Result> r(conn()->execute(std::string("DELETE FROM bdb_files")));
}

namespace {

void
load_queries(const std::string& filename, std::list<std::string>& queries) {
    std::ifstream ifs(filename.c_str(), std::ios::binary);
    std::filebuf* rdbuf = ifs.rdbuf();
    
    char ch;
    std::string str;
    while ((ch = rdbuf->sbumpc()) != EOF) {
        str += ch;
    }
    ifs.close();

     
    boost::split(queries, str, boost::is_any_of(";"), boost::token_compress_on);
}

} // namespace anonymous

void
TestRDB::exec_queries_from(const std::string& file) {
    BoostFileSystem fs;
    std::string qf_path = fs.join(schema_dir_, conn()->dialect().name());
    qf_path = fs.join(qf_path, file);

    std::list<std::string> queries;
    load_queries(qf_path, queries);
    shared_ptr<sql::Connection> c = conn();
    c->begin();
    try {
        scoped_ptr<sql::Result> result;
        BOOST_FOREACH(const std::string& stmt, queries) {
            result.reset(c->execute(stmt));
        }
        c->commit();
    } catch (...) {
        c->rollback();
        throw;
    }
}

} // namespace test
} // namespace brfc
