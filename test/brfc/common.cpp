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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.hpp"

#include <brfc/Variant.hpp>
#include <brfc/test/TestRDB.hpp>

#include <boost/foreach.hpp>

#include <iostream>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "config.hpp"

namespace brfc {

test::TestRDB*
TestRDBEnv::get_database(const char* dsn) {
    test::TestRDB* db = databases_[dsn];
    db->clean();
    return db;
}

void
TestRDBEnv::SetUp() {
#if BRFC_TEST_DSN_COUNT >= 1
    BOOST_FOREACH(const char* dsn, test_dsns) {
        test::TestRDB* db = new test::TestRDB(dsn, test_schema_dir);
        db->drop(); // just in case it's dirty, drops are conditional
        db->create();
        databases_[dsn] = db;
    }
#endif // BRFC_TEST_DSN_COUNT
}

void
TestRDBEnv::TearDown() {
    BOOST_FOREACH(DatabaseMap::value_type& entry, databases_) {
        entry.second->drop();
        delete entry.second;
    }
}

TestRDBEnv::DatabaseMap TestRDBEnv::databases_;

} // namespace brfc

std::ostream& operator<<(std::ostream& out, const QVariant& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const QString& value) {
    out << value.toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const QDate& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const QTime& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Variant& value) {
    out << value.to_qvariant().toString().toStdString();
    return out;
}