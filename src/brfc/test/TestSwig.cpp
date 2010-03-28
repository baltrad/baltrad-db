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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/test/TestSwig.hpp>

namespace brfc {
namespace test {

std::string
TestSwig::qstring_to_string(QString str) {
    return std::string(str.toUtf8().constData());
}

std::string
TestSwig::qstringconstref_to_string(const QString& str) {
    return std::string(str.toUtf8().constData());
}

QString
TestSwig::string_to_qstring(const std::string& str) {
    return QString::fromUtf8(str.c_str());
}

const QString&
TestSwig::string_to_qstringconstref(const std::string& str) {
    for_constref_ = QString::fromUtf8(str.c_str());
    return for_constref_;
}

} // namespace test
} // namespace brfc
