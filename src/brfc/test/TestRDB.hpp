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

#ifndef BRFC_TEST_TEST_RDB_HPP
#define BRFC_TEST_TEST_RDB_HPP

#include <brfc/Path.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>

class StringList;

namespace brfc {
namespace test {

/**
 * @brief a helper class for tests
 *
 * add methods for creating/dropping database schema in a relational database
 */
class TestRDB : public rdb::RelationalDatabase {
  public:
    /**
     * @brief constructor
     *
     * @param dsn database sdn
     * @param schema_dir base directory for finding statements
     */
    TestRDB(const String& dsn, const String& schema_dir);

    virtual ~TestRDB();
    
    /**
     * @brief execute create statements
     */
    void create();

    /**
     * @brief execute drop statements
     */
    void drop();

    /**
     * @brief clear database of all files
     */
    void clean();

  private:
    StringList load_queries(const String& filename);
    void exec_queries_from(const String& filename);

    Path schema_dir_;
};

} // namespace test
} // namespace brfc

#endif // BRFC_TEST_TEST_RDB_HPP 
