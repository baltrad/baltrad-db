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

#ifndef BRFC_DB_RDB_SAVE_FILE_HPP
#define BRFC_DB_RDB_SAVE_FILE_HPP

namespace brfc {

class Oh5Node;
class Oh5PhysicalFile;
class RdbFileEntry;
class RelationalDatabase;

/**
 * @brief save PhysicalFile instances to database
 */
class SaveFile {
  public:
    /**
     * @brief constructor
     * @param rdb the database
     */
    explicit SaveFile(RelationalDatabase* rdb);

    /**
     * @brief save a PhysicalFile instance to database
     * @param file the file to be saved
     * @return file entry in the database
     */
    RdbFileEntry* operator()(const Oh5PhysicalFile& file);

  private:
    Oh5Node& parent_on_entry(const Oh5Node& node);

    RelationalDatabase* rdb_;
};

} // namespace brfc

#endif // BRFC_DB_RDB_SAVE_FILE_HPP
