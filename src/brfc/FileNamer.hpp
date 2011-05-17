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

    namespace oh5 {
        class File;
    }
}

namespace brfc {

/**
 * @brief ABC for generating filenames for File instances
 */
class FileNamer {
  public:
    /**
     * @brief name a File
     * @param file File to be named
     * @return the generated filename
     *
     * @sa do_name(const oh5::File&)
     */
    std::string name(const oh5::File& file) const {
        return do_name(file);
    }
    
    /**
     * @brief name a FileEntry
     * @param entry FileEntry to be named
     * @return the generated filename
     *
     * @sa do_name(const FileEntry&)
     */
    std::string name(const FileEntry& entry) const {
        return do_name(entry);
    }

    /**
     * @brief destructor
     */
    virtual ~FileNamer() { }

  protected:
    /**
     * @brief name(const oh5::File&) implementation
     */
    virtual std::string do_name(const oh5::File& file) const = 0;

    /**
     * @brief name(const oh5::File&) implementation
     */
    virtual std::string do_name(const FileEntry& entry) const = 0;
};

} // namespace brfc

#endif // BRFC_FILE_NAMER_HPP
