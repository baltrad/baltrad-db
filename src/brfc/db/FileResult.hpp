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

#ifndef BRFC_DB_FILE_RESULT_HPP
#define BRFC_DB_FILE_RESULT_HPP

#include <boost/noncopyable.hpp>

namespace brfc {

class FileEntry;

/**
 * @brief ABC for FileQuery results
 */
class FileResult : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~FileResult() { }

    /**
     * @brief get number of entries in result
     */
    int size() const { return do_size(); }
    
    /**
     * @brief move to next entry
     * @return true if there is a valid entry
     */
    bool next() { return do_next(); }

    /**
     * @brief seek to entry
     * @param idx entry number, starting from 0. if negative, seek backwards
     * @return true if new row is valid
     */
    bool seek(int idx) {
        return do_seek(idx);
    }
    
    /**
     * @brief get the entry
     * @note caller takes owneship of the entry
     */
    FileEntry* entry() { return do_entry(); }

  protected:
    /**
     * @brief size() implementation
     */
    virtual int do_size() const = 0;

    /**
     * @brief next() implementation
     */
    virtual bool do_next() = 0;
    
    /**
     * @brief seek() implementation
     */
    virtual bool do_seek(int idx) {
        return do_seek(idx);
    }

    /**
     * @brief entry() implementation
     */
    virtual FileEntry* do_entry() = 0;
};

} // namespace brfc

#endif // BRFC_FILE_RESULT_HPP
