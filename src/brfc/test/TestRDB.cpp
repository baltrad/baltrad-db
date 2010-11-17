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

namespace fs = boost::filesystem;

namespace brfc {
namespace test {

TestRDB::TestRDB(const String& dsn, const String& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(schema_dir.to_utf8()) {
    if (not fs::path(schema_dir_).is_complete())
        throw value_error("schema dir must be a complete path");
}

TestRDB::~TestRDB() {

}

void
TestRDB::create() {
    exec_queries_from(String("create.sql"));
}

void
TestRDB::drop() {
    exec_queries_from(String("drop.sql"));
}

void
TestRDB::clean() {
    conn()->execute(String("DELETE FROM bdb_files"));
}

StringList
TestRDB::load_queries(const String& filename) {
    fs::path qf_path = fs::path(schema_dir_)
                       / conn()->dialect().name().to_utf8()
                       / filename.to_utf8();
    
    std::ifstream ifs(qf_path.string().c_str(), std::ios::binary);
    std::filebuf* rdbuf = ifs.rdbuf();
    
    char ch;
    std::string str;
    while ((ch = rdbuf->sbumpc()) != EOF) {
        str += ch;
    }
    ifs.close();

    String content = String::from_utf8(str);
    return content.split(";\n", String::SKIP_EMPTY_PARTS);
}

void
TestRDB::exec_queries_from(const String& file) {
    const StringList& queries = load_queries(file);
    shared_ptr<sql::Connection> c = conn();
    c->begin();
    try {
        BOOST_FOREACH(const String& stmt, queries) {
            c->execute(stmt);
        }
        c->commit();
    } catch (...) {
        c->rollback();
        throw;
    }
}

} // namespace test
} // namespace brfc
