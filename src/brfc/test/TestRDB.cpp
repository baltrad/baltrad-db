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
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtSql/QSqlQuery>

#include <brfc/rdb/Connection.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace test {

TestRDB::TestRDB(const QString& dsn, const QString& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(schema_dir) {
    schema_dir_.makeAbsolute();
}

TestRDB::~TestRDB() {

}

void
TestRDB::create() {
    exec_queries_from(QString("create.sql"));
    populate_mapper_and_specs();
}

void
TestRDB::drop() {
    exec_queries_from(QString("drop.sql"));
}

void
TestRDB::clean() {
    connection().execute(QString("DELETE FROM bdb_files"));
}

QStringList
TestRDB::load_queries(const QString& filename) {
    QString path = schema_dir_.path() + QString("/") + dialect() + QString("/") + filename;
    QFile file(path);
    if (not file.open(QIODevice::ReadOnly)) {
        throw fs_error("could not open file: " + path.toStdString());
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    const QString content = stream.readAll();
    return content.split(";\n", QString::SkipEmptyParts);
}

void
TestRDB::exec_queries_from(const QString& file) {
    const QStringList& queries = load_queries(file);
    begin();
    try {
        BOOST_FOREACH(const QString& stmt, queries) {
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
