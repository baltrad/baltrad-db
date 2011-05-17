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

#ifndef BRFC_FUSE_FILE_FACTORY_HPP
#define BRFC_FUSE_FILE_FACTORY_HPP

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>

#include <brfc/smart_ptr.hpp>

#include <brfc/expr/Expression.hpp>

#include <brfc/fuse/EntryFactory.hpp>
#include <brfc/fuse/FileEntry.hpp>

namespace brfc {

class FileNamer;

namespace db {
    class Database;
    class FileEntry;
} // namespace db

namespace fuse {

class FileEntry;

class FileFactory : public EntryFactory {
  public:
    /**
     * @brief constructor
     * @param db the database to query from
     * @param namer namer to use for naming files
     */
    FileFactory(db::Database* db, FileNamer* namer);
    
    virtual ~FileFactory();
  
  protected:
    FileFactory(const FileFactory& other);
  
  private:
    virtual FileFactory* do_clone() const;

    void do_filter(const Expression& expr);

    virtual void do_update();

    virtual std::vector<std::string> do_entry_names() const;

    virtual Entry* do_entry_by_name(const std::string&) const;

    struct set_valid {
        set_valid(bool valid) : valid_(valid) { }
        void operator()(shared_ptr<FileEntry>& entry) const {
            entry->valid(valid_);
        }

      private:
        bool valid_;
    };

    struct by_uuid {};
    struct by_name {};
    struct by_valid {};

    typedef boost::multi_index_container<
        shared_ptr<FileEntry>,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<by_uuid>,
                boost::multi_index::const_mem_fun<
                    FileEntry, std::string, &FileEntry::uuid>
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<by_name>,
                boost::multi_index::const_mem_fun<
                    FileEntry, std::string, &FileEntry::name>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<by_valid>,
                boost::multi_index::const_mem_fun<
                    FileEntry, bool, &FileEntry::valid>
            >
        >
    > EntrySet_t;

    typedef EntrySet_t::index<by_uuid>::type EntryByUuid_t;
    typedef EntrySet_t::index<by_name>::type EntryByName_t;
    typedef EntrySet_t::index<by_valid>::type EntryByValid_t;

    db::Database& database() { return *db_; }
    FileNamer& namer() { return *namer_; }

    void invalidate_all();
    void remove_invalid();

    shared_ptr<FileEntry> create_entry(const db::FileEntry& fe);
    
    db::Database* db_; 
    FileNamer* namer_;
    Expression filter_;
    EntrySet_t entries_;
};

} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_FILE_FACTORY_HPP
