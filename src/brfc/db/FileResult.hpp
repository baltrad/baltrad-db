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

#ifndef BRFC_DB_FILE_RESULT_HPP
#define BRFC_DB_FILE_RESULT_HPP

#include <vector>

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

namespace brfc {
namespace db {

class FileEntry;

/**
 * @brief FileQuery results
 */
class FileResult : public boost::noncopyable {
  public:
    FileResult()
        : entries_() {
    }

    /**
     * @brief destructor
     */
    virtual ~FileResult() { }

    /**
     * @brief get number of rows in result
     */
    int size() {
        return entries_.size();
    }

    shared_ptr<FileEntry> get(int i) {
        return entries_.at(i);
    }

    void add(shared_ptr<FileEntry> e) {
        entries_.push_back(e);
    }
    
  private:
    std::vector<shared_ptr<FileEntry> > entries_;
};

} // namespace db
} // namespace brfc

#endif // BRFC_FILE_RESULT_HPP
