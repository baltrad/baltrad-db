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

#ifndef BRFC_DEFAULT_FILE_NAMER_HPP
#define BRFC_DEFAULT_FILE_NAMER_HPP

#include <brfc/FileNamer.hpp>

namespace brfc {

/**
 * @brief default FileNamer implementation
 *
 * name files as Z_\$(type)_C_\$(CCCC)_ \$(timestamp)_\$(freeformat).h5
 *
 * where:
 *   - @c type is the value of "/what/object"
 *   - @c CCCC is the WMO CCCC associated with the Source
 *   - @c timestamp is the value of "/what/date" and "/what/time" formatted
 *     as @c yyyyMMddhhmmss
 *   - @c freeformat contains the node_id associated with the Source
 */
class DefaultFileNamer : public FileNamer {
  public:
    DefaultFileNamer();

  protected:
    virtual String do_name(const oh5::File& f) const;
};

} // namespace brfc

#endif // BRFC_DEFAULT_FILE_NAMER_HPP
