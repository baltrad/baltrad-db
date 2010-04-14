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

#ifndef BRFC_FILE_NAMER_HPP
#define BRFC_FILE_NAMER_HPP

#include <QtCore/QString>

namespace brfc {

namespace oh5 {
    class File;
}

/**
 * @brief ABC for generating filenames for File instances
 */
class FileNamer {
  public:
    /**
     * @brief inject version information into a filename
     *
     * @return modified filename
     *
     * this method tries to do one of the following, in the specified order:
     * - if filename contains a string '$VERSION$', it is replaced with the
     *   version
     * - if filename has one or more '.' in it, the last dot is taken to be
     *   an extension separator and version is placed right before it,
     *   prefixed with '_'
     * - otherwise, version is put at the end of the string, prefixed with '_'
     *
     * The version inserted into the string is pre-padded with at most 6 zero
     * digits.
     */
    static QString inject_version(const QString& filename,
                                  unsigned int version);

    /**
     * @brief name a File
     * @param file File to be named
     * @return the generated filename
     *
     * @sa do_name
     */
    QString name(const oh5::File& file) const {
        return do_name(file);
    }

    /**
     * @brief destructor
     */
    virtual ~FileNamer() { }

  protected:
    virtual QString do_name(const oh5::File& file) const = 0;
};

} // namespace brfc

#endif // BRFC_FILE_NAMER_HPP
