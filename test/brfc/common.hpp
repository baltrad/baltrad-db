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

#ifndef BRFC_TEST_COMMON_HPP
#define BRFC_TEST_COMMON_HPP

#include <iosfwd>
#include <map>

#include <gtest/gtest.h>

class QString;

namespace brfc {

class Date;
class Time;
class Variant;

namespace test {

class TestRDB;

}

class TestRDBEnv : public ::testing::Environment {
  public:
    
    virtual ~TestRDBEnv() { }

    static test::TestRDB* get_database(const char* dsn);

    virtual void SetUp();

    virtual void TearDown();

  private:
    typedef std::map<const char*, test::TestRDB*> DatabaseMap;
    static DatabaseMap databases_;
};

} // namespace brfc

std::ostream&
operator<<(std::ostream& out, const QString& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Date& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Time& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Variant& value);

#endif // BRFC_TEST_COMMON_HPP
