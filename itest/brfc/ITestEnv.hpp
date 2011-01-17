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

#include <map>

#include <gtest/gtest.h>

namespace brfc {

namespace test {
    class TestRDB;
} // namespace test

class ITestEnv : public ::testing::Environment {
  public:
    
    virtual ~ITestEnv() { }

    static test::TestRDB* get_database(const char* dsn);

    virtual void SetUp();

    virtual void TearDown();

  private:
    typedef std::map<const char*, test::TestRDB*> DatabaseMap;

    static DatabaseMap databases_;
};


} // namespace brfc
