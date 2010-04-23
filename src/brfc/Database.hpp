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

#ifndef BRFC_DATABASE_H
#define BRFC_DATABASE_H

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

class QString;

namespace brfc {

class ResultSet;
class Query;

namespace oh5 {

class File;
class Source;

}

/**
 * @brief ABC for Database access
 */
class Database : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~Database() { }
    
    /**
     * @brief start a transaction
     */
    void begin() {
        do_begin();
    }

    /**
     * @brief roll back current transaction
     */
    void rollback() {
        do_rollback();
    }

    /**
     * @brief commit current transaction
     */
    void commit() {
        do_commit();
    }

    /**
     * @brief is file stored
     */
    bool has_file(const oh5::File& file) {
        return do_has_file(file);
    }
    
    /**
     * @brief remove file from database
     */
    void remove_file(const QString& path) {
        do_remove_file(path);
    }

    /**
     * @brief save file to database
     * @param file the file to be saved
     * @param proposed_filename filename proposed by a namer, a string for
     *        which filename_version is valid.
     * @param filename_version filename version associated with proposed
     *        filename
     * @return database id associated with the file
     */
    long long save_file(const oh5::File& file,
                        const QString& proposed_filename,
                        unsigned int filename_version) {
        return do_save_file(file, proposed_filename, filename_version);
    }

    /**
     * @brief get the database id of a file
     */
    long long db_id(const oh5::File& file) {
        return do_db_id(file);
    }
    
    /**
     * @brief next version number for a filename
     */
    unsigned int next_filename_version(const QString& filename) {
        return do_next_filename_version(filename);
    }

    /**
     * @brief load source from DB by contents of /what/source
     */
    shared_ptr<oh5::Source> load_source(const QString& source) {
        return do_load_source(source);
    }
    
    /**
     * @brief execute a select query
     */
    shared_ptr<ResultSet> query(const Query& query) {
        return do_query(query);
    }

  protected:
    virtual void do_begin() = 0;
    virtual void do_rollback() = 0;
    virtual void do_commit() = 0;
    
    virtual bool do_has_file(const oh5::File& file) = 0;
    virtual void do_remove_file(const QString& path) = 0;
    virtual long long do_save_file(const oh5::File& file,
                                   const QString& proposed_filename,
                                   unsigned int filename_version) = 0;
    virtual long long do_db_id(const oh5::File& file) = 0;

    virtual unsigned int do_next_filename_version(const QString& filename) = 0;

    virtual shared_ptr<oh5::Source> do_load_source(const QString& source) = 0;

    virtual shared_ptr<ResultSet> do_query(const Query& query) = 0;
};

} // namespace brfc

#endif // BRFC_DATABASE_H
