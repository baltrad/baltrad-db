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

#include <iostream>

#include <boost/foreach.hpp>

#include <brfc/Date.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Scalar.hpp>
#include <brfc/test/TestRDB.hpp>

#include <brfc/test_config.hpp>
#include <brfc/test_common.hpp>

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

std::ostream& operator<<(std::ostream& out, const brfc::Date& value) {
    out << "Date("
        << value.year() << ", "
        << value.month() << ", "
        << value.day() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Time& value) {
    out << "Time("
        << value.hour() << ", "
        << value.minute() << ", "
        << value.second() << ", "
        << value.msec() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::DateTime& value) {
    out << "DateTime("
        << value.date().year() << ", "
        << value.date().month() << ", "
        << value.date().day() << ", "
        << value.time().hour() << ", "
        << value.time().minute() << ", "
        << value.time().second() << ", "
        << value.time().msec() << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Variant& value) {
    out << value.to_string().to_std_string();
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::oh5::Scalar& value) {
    out << "oh5::Scalar("
        << value.to_string().to_std_string()
        << ")";
    return out;
}
