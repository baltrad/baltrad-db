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

#ifndef BRFC_FUSE_ENTRY_FACTORY_HPP
#define BRFC_FUSE_ENTRY_FACTORY_HPP

#include <string>
#include <vector>

namespace brfc {
    class Expression;
}

namespace brfc {
namespace fuse {

class Entry;

class EntryFactory {
  public:
    EntryFactory() { }

    virtual ~EntryFactory() { }
    
    /**
     * @brief clone this factory
     */
    EntryFactory* clone() const {
        return do_clone();
    }
    
    /**
     * @brief set the filter to use on database queries
     */
    void filter(const Expression& expr) {
        do_filter(expr);
    }
    
    /**
     * @brief update from database
     */
    void update() {
        do_update();
    }
    
    /**
     * @brief get a list of entry names created in this factory
     */
    std::vector<std::string> entry_names() const {
        return do_entry_names();
    }
    
    /**
     * @brief get a child entry by name
     * @return pointer to the child entry or null pointer if not found
     */
    Entry* entry_by_name(const std::string& name) const {
        return do_entry_by_name(name);
    }
  
  protected:
    EntryFactory(const EntryFactory& other);
  
  private:
    EntryFactory& operator=(const EntryFactory& rhs); 

    virtual EntryFactory* do_clone() const = 0;

    virtual void do_filter(const Expression& expr) = 0;

    virtual void do_update() = 0;

    virtual std::vector<std::string> do_entry_names() const = 0;
    
    virtual Entry* do_entry_by_name(const std::string& name) const = 0;
};

} // namespace fuse
} // namespace brfc

#endif // BRFC_FUSE_ENTRY_FACTORY_HPP
