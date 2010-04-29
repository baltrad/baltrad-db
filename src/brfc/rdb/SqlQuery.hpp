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

#ifndef BRFC_RDB_SQL_QUERY_HPP
#define BRFC_RDB_SQL_QUERY_HPP

#include <QtCore/QString>

#include <brfc/rdb/BindMap.hpp>

namespace brfc {
namespace rdb {

class SqlQuery {
  public:
    explicit SqlQuery(const QString& statement,
                      const BindMap& binds=BindMap())
            : statement_(statement)
            , binds_(binds) {
    }

    void binds(const BindMap& binds) {
        binds_ = binds;
    }

    BindMap& binds() {
        return binds_;
    }

    const BindMap& binds() const {
        return binds_;
    }

    void statement(const QString& statement) {
        statement_ = statement;
    }

    QString& statement() {
        return statement_;
    }

    const QString& statement() const {
        return statement_;
    }

  private:
    QString statement_;
    BindMap binds_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SQL_QUERY_HPP