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
#ifndef BRFC_RDB_IN_DATABASE_FILE_STORAGE_HPP
#define BRFC_RDB_IN_DATABASE_FILE_STORAGE_HPP

#include <brfc/rdb/RdbFileStoragePolicy.hpp>

namespace brfc {

class RdbInDatabaseFileStorage : public RdbFileStoragePolicy {
  public:
    RdbInDatabaseFileStorage();
    virtual ~RdbInDatabaseFileStorage();

    using RdbFileStoragePolicy::database;
    RelationalDatabase* database();
  
  private:
    virtual void do_database(RelationalDatabase* db);

    virtual void do_store(RdbFileEntry& e, const std::string& path);

    virtual bool do_remove(const RdbFileEntry& e);

    virtual void do_write_to_file(const RdbFileEntry& e,
                                  const std::string& path);
    
    RelationalDatabase* db_;
};

} // namespace brfc

#endif // BRFC_RDB_IN_DATABASE_FILE_STORAGE_HPP
