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

#ifndef BRFC_SQL_BIND_MAP_HPP
#define BRFC_SQL_BIND_MAP_HPP

#include <map>
#include <brfc/Variant.hpp>

namespace brfc {
namespace sql {

class BindMap {
  public:
    typedef std::map<std::string, Variant> map;
    typedef map::iterator iterator;
    typedef map::const_iterator const_iterator;
    typedef map::value_type value_type;

    BindMap();

    BindMap(const BindMap& other);

    BindMap& operator=(const BindMap& rhs);

    /**
     * @throw duplicate_entry if bind already exists
     */
    void add(const std::string& name, const Variant& value);

    /**
     * @throw lookup_error if bind doesn't exist
     *
     */
    void set(const std::string& name, const Variant& value);
    
    /**
     * @return true if bind exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @throw lookup_error if bind not found
     */
    const Variant& get(const std::string& name) const;
    
    /**
     * @brief get bind value
     * @return the value or default_ if bind not found
     */
    const Variant& get(const std::string& name, const Variant& default_) const;

    size_t size() const {
        return binds_.size();
    }

    iterator begin() {
        return binds_.begin();
    }

    const_iterator begin() const {
        return binds_.begin();
    }

    iterator end() {
        return binds_.end();
    }

    const_iterator end() const {
        return binds_.end();
    }

    /**
     * @brief remove a bind
     */
    bool remove(const std::string& name);
    
    void clear() {
        binds_.clear();
    }

  private:
    std::string name_to_placeholder(const std::string& name) const;

    map binds_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_BIND_MAP_HPP
