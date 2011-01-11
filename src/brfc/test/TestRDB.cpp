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

#include <brfc/test/TestRDB.hpp>

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/StringList.hpp>

#include <brfc/sql/Connection.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Result.hpp>

namespace fs = boost::filesystem;

namespace brfc {
namespace test {

TestRDB::TestRDB(const std::string& dsn, const std::string& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(schema_dir) {
    if (not fs::path(schema_dir_).is_complete())
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
    scoped_ptr<sql::Result> r(conn()->execute("DELETE FROM bdb_files"));
}

StringList
TestRDB::load_queries(const std::string& filename) {
    fs::path qf_path = fs::path(schema_dir_)
                       / conn()->dialect().name()
                       / filename;
    
    std::ifstream ifs(qf_path.string().c_str(), std::ios::binary);
    std::filebuf* rdbuf = ifs.rdbuf();
    
    char ch;
    std::string str;
    while ((ch = rdbuf->sbumpc()) != EOF) {
        str += ch;
    }
    ifs.close();

    return StringList::split(str, ";", StringList::SKIP_EMPTY_PARTS);
}

void
TestRDB::exec_queries_from(const std::string& file) {
    const StringList& queries = load_queries(file);
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
