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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_QSQL_DATABASE_H
#define BRFC_QSQL_DATABASE_H

#include <string>
#include <map>

#include <QtCore/QString>
#include <QtSql/QSqlDatabase>

#include <brfc/smart_ptr.hpp>
#include <brfc/Database.hpp>

class QVariant;
class QCoreApplication;

namespace brfc {

class FileHasher;
class Variant;

namespace oh5 {

class AttributeSpecs;
class File;
class Source;
class SourceCentre;
class SourceRadar;

}

namespace rdb {

class AttributeMapper;
class RelationalResultSet;

typedef std::map<QString, Variant> BindMap;
typedef std::map<weak_ptr<void>, long long> IdCache;

/**
 * @brief Database using QtSql
 */
class RelationalDatabase : public Database {
  public:
    typedef QVariant id_type;

    /**
     * @brief constructor
     * @param dsn connection  in URI-like syntax: 
     *            <tt> engine://user:password\@host:port/dbname </tt>
     *
     * the only engine currently supported is 'postgresql'
     */
    explicit RelationalDatabase(const QString& dsn);
    
    /**
     * @brief destructor
     */
    virtual ~RelationalDatabase();

    /**
     * @brief execute a select query
     */
    shared_ptr<RelationalResultSet> query(const QString& query,
                                          const BindMap& binds);
    
    shared_ptr<const oh5::AttributeSpecs> specs() const;

    shared_ptr<oh5::AttributeSpecs> specs();

    const AttributeMapper& mapper() const;

    void populate_mapper_and_specs();

    QSqlDatabase& connection() {
        return *sql_;
    }

    bool supports_returning() const {
        return supports_returning_;
    }

    void file_hasher(shared_ptr<FileHasher> hasher);
    
    /**
     * @note caller retains hasher ownership
     */
    void file_hasher(FileHasher* hasher);

  protected:
    virtual void do_begin();
    virtual void do_rollback();
    virtual void do_commit();

    /**
     * @brief check if file hash is unique in database
     */
    virtual bool do_has_file(const oh5::File& file);
    virtual void do_remove_file(const QString& path);

    virtual long long do_save_file(const oh5::File& file,
                                   const QString& proposed_filename,
                                   unsigned int filename_version);

    virtual unsigned int do_next_filename_version(const QString& filename);

    virtual shared_ptr<oh5::Source> do_load_source(const QString& srcstr);
    
    virtual shared_ptr<ResultSet> do_query(const Query& query);

    QString dialect() const { return dialect_; }
  
  private:
    void init();

    void init_qapp();

    void load_source_radar(shared_ptr<oh5::SourceRadar> src);
    void load_source_centre(shared_ptr<oh5::SourceCentre> src);

    /**
     * @brief save file to database
     */
    id_type save(const oh5::File& file,
                 const QString& proposed_filename,
                 unsigned int filename_version);

    /**
     * @brief remove file from database
     */
    void remove_file(const QString& path);
    
    QString qt_engine(const QString& engine) const;

    shared_ptr<QSqlDatabase> sql_;
    scoped_ptr<AttributeMapper> mapper_;
    shared_ptr<oh5::AttributeSpecs> specs_;
    shared_ptr<FileHasher> file_hasher_;
    QString dialect_;
    bool supports_returning_;
    IdCache id_cache_;
    static unsigned int connection_count_;
    static int argc_;
    static const char* argv_[];
    static QCoreApplication* qapp_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_QT_SQL_DATABASE_H
