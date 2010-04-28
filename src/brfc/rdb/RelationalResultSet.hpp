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

#ifndef BRFC_RELATIONAL_RESULT_SET_HPP
#define BRFC_RELATIONAL_RESULT_SET_HPP

#include <QtSql/QSqlDatabase>

#include <brfc/smart_ptr.hpp>
#include <brfc/ResultSet.hpp>

class QDate;
class QTime;
class QSqlDatabase;
class QSqlQuery;

namespace brfc {
namespace rdb {

/**
 * @brief results from RelationalDatabase
 */
class RelationalResultSet : public ResultSet {
  public:
    /**
     * @brief construct from QSqlQuery
     *
     * position is before first row.
     */
    explicit RelationalResultSet(const QSqlQuery& query,
                                 shared_ptr<QSqlDatabase> db);
    
    /**
     * @brief destructor
     */
    virtual ~RelationalResultSet();
  
  protected:
    virtual bool do_next();
    virtual bool do_seek(int idx);
    virtual int do_size();

    Variant value_at(unsigned int post) const;

    virtual QString do_string(unsigned int pos) const;
    virtual long long do_integer(unsigned int pos) const;
    virtual double do_real(unsigned int pos) const;
    virtual bool do_boolean(unsigned int pos) const;
    virtual QDate do_date(unsigned int pos) const;
    virtual QTime do_time(unsigned int pos) const;

  private:
    shared_ptr<QSqlDatabase> db_;
    scoped_ptr<QSqlQuery> query_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RELATIONAL_RESULT_SET_HPP
