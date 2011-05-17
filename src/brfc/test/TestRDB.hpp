/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <string>
#include <brfc/db/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace test {

/**
 * @brief a helper class for tests
 *
 * add methods for creating/dropping database schema in a relational database
 */
class TestRDB : public RelationalDatabase {
  public:
    /**
     * @brief constructor
     *
     * @param dsn database sdn
     * @param schema_dir base directory for finding statements
     */
    TestRDB(const std::string& dsn, const std::string& schema_dir);

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
    void exec_queries_from(const std::string& filename);

    std::string schema_dir_;
};

} // namespace test
} // namespace brfc

#endif // BRFC_TEST_TEST_RDB_HPP 
