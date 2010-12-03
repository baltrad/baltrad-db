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

#ifndef BRFC_SQL_LARGE_OBJECT_HPP
#define BRFC_SQL_LARGE_OBJECT_HPP

namespace brfc {

class String;

namespace sql {

class LargeObject {
  public:
    virtual ~LargeObject() { }
  
    long long id() const { return do_id(); }

    void write_to_file(const std::string& path) const {
        do_write_to_file(path);
    }

  protected:
    virtual long long do_id() const = 0;
    virtual void do_write_to_file(const std::string& path) const = 0;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_LARGE_OBJECT_HPP
