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

#include <boost/foreach.hpp>

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <brfc/StringList.hpp>

#include <brfc/rdb/Connection.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace test {

TestRDB::TestRDB(const String& dsn, const String& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(schema_dir) {
    if (not schema_dir_.is_absolute())
        throw value_error("schema dir must be absolute");
}

TestRDB::~TestRDB() {

}

void
TestRDB::create() {
    exec_queries_from(String("create.sql"));
    populate_mapper_and_specs();
}

void
TestRDB::drop() {
    exec_queries_from(String("drop.sql"));
}

void
TestRDB::clean() {
    connection().execute(String("DELETE FROM bdb_files"));
}

StringList
TestRDB::load_queries(const String& filename) {
    String path = schema_dir_.join(dialect()).join(filename).string();
    QString qpath = QString::fromUtf8(path.to_utf8().c_str());
    QFile file(qpath);
    if (not file.open(QIODevice::ReadOnly)) {
        throw fs_error("could not open file: " + path.to_std_string());
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    String content = String::from_utf16(stream.readAll().utf16());
    return content.split(";\n", String::SKIP_EMPTY_PARTS);
}

void
TestRDB::exec_queries_from(const String& file) {
    const StringList& queries = load_queries(file);
    begin();
    try {
        BOOST_FOREACH(const String& stmt, queries) {
            connection().execute(stmt);
        }
        commit();
    } catch (...) {
        rollback();
        throw;
    }
}

} // namespace test
} // namespace brfc
