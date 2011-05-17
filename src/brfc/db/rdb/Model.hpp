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

#ifndef BRFC_DB_RDB_MODEL_HPP
#define BRFC_DB_RDB_MODEL_HPP

#include <string>
#include <brfc/expr/Listcons.hpp>

namespace brfc {
namespace db {
namespace rdb {


struct m {

    struct sources {
        static std::string name() { return "bdb_sources"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };

    struct source_kvs {
        static std::string name() { return "bdb_source_kvs"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };

    struct files {
        static std::string name() { return "bdb_files"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };

    struct file_content {
        static std::string name() { return "bdb_file_content"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };

    struct nodes {
        static std::string name() { return "bdb_nodes"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };

    struct attrvals {
        static std::string name() { return "bdb_attribute_values"; }

        static Expression column(const std::string& colname) {
            return Listcons().symbol("column")
                                   .string(name())
                                   .string(colname)
                                   .get();
        }
    };
};

} // namespace rdb
} // namespace db
} // namespace brfc

#endif // BRFC_DB_RDB_MODEL_HPP
