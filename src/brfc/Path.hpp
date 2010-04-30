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

#ifndef BRFC_PATH_HPP
#define BRFC_PATH_HPP

#include <QtCore/QString>

namespace brfc {

class Path {
  public:
    explicit Path(const QString& path);

    bool exists() const;
    
    bool is_absolute() const;

    bool is_dir() const;

    Path join(const QString& path) const;

    const QString& string() const {
        return path_;
    }

  private:
    QString path_;
    static QString sep_;
};

} // namespace brfc

#endif // BRFC_PATH_HPP
