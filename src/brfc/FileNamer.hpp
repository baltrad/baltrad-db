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

#ifndef BRFC_FILE_NAMER_HPP
#define BRFC_FILE_NAMER_HPP

#include <string>

namespace brfc {

    class FileEntry;
    class Oh5File;
}

namespace brfc {

/**
 * @brief ABC for generating filenames for File instances
 * @ingroup exposed
 */
class FileNamer {
  public:
    /**
     * @brief destructor
     */
    virtual ~FileNamer() = 0;

    /**
     * @brief name a file
     * @param file file to be named
     * @return the generated filename
     */
    std::string name(const Oh5File& file) const {
        return do_name(file);
    }

    /**
     * @brief name a file entry
     * @param entry file entry to be named
     * @return the generated filename
     */
    std::string name(const FileEntry& entry) const {
        return do_name(entry);
    }
    
  protected:
    virtual std::string do_name(const Oh5File& file) const = 0;

    virtual std::string do_name(const FileEntry& entry) const = 0;

};

inline FileNamer::~FileNamer() { }

} // namespace brfc

#endif // BRFC_FILE_NAMER_HPP
