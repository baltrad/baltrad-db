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

#include <QtCore/QRegExp>

#include <brfc/String.hpp>

namespace brfc {

class RegExp {
  public:
    RegExp(const String& pattern)
            : re_(QString::fromUtf8(pattern.to_utf8().c_str())) {

    }

    int index_in(const String& str, int pos) {
        return re_.indexIn(QString::fromUtf8(str.to_utf8().c_str()), pos);
    }

    int matched_length() const {
        return re_.matchedLength();
    }

    String cap() const {
        return re_.cap();
    }

    int pos() const {
        return re_.pos();
    }

  private:
    QRegExp re_;
};

} // namespace brfc
