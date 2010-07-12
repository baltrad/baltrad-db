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

#ifndef BRFC_SQL_FROM_CLAUSE_HPP
#define BRFC_SQL_FROM_CLAUSE_HPP

#include <vector>

#include <brfc/sql/Element.hpp>

#include <brfc/sql/fwd.hpp>

namespace brfc {
namespace sql {

class FromClause : public Element {
  public:
    static FromClausePtr create() {
        return FromClausePtr(new FromClause());
    }
    
    /**
     * @throw duplicate_entry if not unique
     */
    void add(SelectablePtr selectable);
    
    /**
     * @return true if already has a selectable with same name
     */
    bool has(SelectablePtr selectable) const;

    std::vector<SelectablePtr>& elements() { return elements_; }

    bool empty() const {
        return elements_.empty();
    }

  protected:
    FromClause()
            : elements_() {
    }

  private:
    std::vector<SelectablePtr> elements_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_FROM_CLAUSE_HPP
