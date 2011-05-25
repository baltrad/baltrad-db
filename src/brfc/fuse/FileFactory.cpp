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

#include <brfc/fuse/FileFactory.hpp>

#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

#include <brfc/assert.hpp>
#include <brfc/FileNamer.hpp>

#include <brfc/db/Database.hpp>
#include <brfc/db/FileEntry.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/db/FileResult.hpp>

namespace brfc {
namespace fuse {

FileFactory::FileFactory(Database* db,
                         FileNamer* namer)
        : db_(db)
        , namer_(namer)
        , filter_()
        , entries_() {
    BRFC_ASSERT(db_);
    BRFC_ASSERT(namer_);
}

FileFactory::FileFactory(const FileFactory& other)
        : db_(other.db_)
        , namer_(other.namer_)
        , filter_(other.filter_)
        , entries_(other.entries_) {

}

FileFactory::~FileFactory() {

}

FileFactory*
FileFactory::do_clone() const {
    return new FileFactory(*this);
}

void
FileFactory::do_filter(const Expression& expr) {
    filter_ = expr;
}

void
FileFactory::do_update() {
    invalidate_all();

    FileQuery qry;
    if (not filter_.empty())
        qry.filter(filter_);
    boost::scoped_ptr<FileResult> result(database().execute(qry));

    EntryByUuid_t& es = entries_.get<by_uuid>();
    EntryByUuid_t::iterator iter;
    
    boost::scoped_ptr< ::brfc::FileEntry> fe;
    while (result->next()) {
        fe.reset(result->entry());
        const std::string& uuid = fe->uuid();
        iter = es.find(uuid);
        if (iter != es.end()) {
            es.modify(iter, set_valid(true));
        } else {
            es.insert(create_entry(*fe));
        }
    }
    
    remove_invalid();
}

std::vector<std::string>
FileFactory::do_entry_names() const {
    std::vector<std::string> names;
    names.reserve(entries_.size());
    for (EntrySet_t::const_iterator iter = entries_.begin();
         iter != entries_.end();
         ++iter) {
        names.push_back((*iter)->name());
    }

    return names;
}

Entry*
FileFactory::do_entry_by_name(const std::string& name) const {
    const EntryByName_t& es = entries_.get<by_name>();
    EntryByName_t::const_iterator it = es.find(name);
    return it != es.end() ? (*it).get() : 0;
}

void
FileFactory::invalidate_all() {
    EntryByUuid_t& es = entries_.get<by_uuid>();

    EntryByUuid_t::iterator iter;
    for (iter = es.begin(); iter != es.end(); ++iter) {
        es.modify(iter, set_valid(false));
    }
}

void
FileFactory::remove_invalid() {
    EntryByValid_t& es = entries_.get<by_valid>();
    es.erase(es.lower_bound(false), es.upper_bound(false));
}

boost::shared_ptr<FileEntry>
FileFactory::create_entry(const ::brfc::FileEntry& fe) {
    const std::string& name = namer().name(fe);
    return boost::make_shared<FileEntry>(db_, fe, name);
}


} // namespace fuse
} // namespace brfc
