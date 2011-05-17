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

#ifndef BRFC_DEFAULT_FILE_NAMER_HPP
#define BRFC_DEFAULT_FILE_NAMER_HPP

#include <brfc/FileNamer.hpp>

namespace brfc {

/**
 * @brief default FileNamer implementation
 *
 */
class DefaultFileNamer : public FileNamer {
  public:
    DefaultFileNamer();

  protected:
    /**
     * name files as \$(type)_\$(source)_\$(timestamp).h5
     *
     * where:
     *   - @c type is the value of "/what/object"
     *   - @c source is the value of 'name' key in Source or 'unknown'
     *   - @c timestamp is the value of "/what/date" and "/what/time" formatted
     *     as @c yyyyMMddThhmmssZ
     */
    virtual std::string do_name(const Oh5File& file) const;

    /**
     * @brief name a FileEntry
     *
     * use the name given by name(const File&), but inject the first group
     * of the uuid before the file extension.
     */
    virtual std::string do_name(const FileEntry& entry) const;
};

} // namespace brfc

#endif // BRFC_DEFAULT_FILE_NAMER_HPP
