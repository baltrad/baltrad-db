#include "common.hpp"

#include <brfc/TestRDB.hpp>
#include <brfc/Variant.hpp>

#include <boost/foreach.hpp>

#include <iostream>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "config.hpp"

namespace brfc {

TestRDB*
TestRDBEnv::get_database(const char* dsn) {
    TestRDB* db = databases_[dsn];
    db->clean();
    return db;
}

void
TestRDBEnv::SetUp() {
    BOOST_FOREACH(const char* dsn, test_dsns) {
        TestRDB* db = new TestRDB(dsn, test_schema_dir);
        db->create();
        databases_[dsn] = db;
    }
}

void
TestRDBEnv::TearDown() {
    BOOST_FOREACH(const char* dsn, test_dsns) {
        TestRDB* db = databases_[dsn];
        db->drop();
        delete db;
    }
}

std::map<const char*, TestRDB*> TestRDBEnv::databases_;

} // namespace brfc

std::ostream& operator<<(std::ostream& out, const QVariant& value) {
    out << value.toString().toStdString();
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
