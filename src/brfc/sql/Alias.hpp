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

#ifndef BRFC_SQL_ALIAS_HPP
#define BRFC_SQL_ALIAS_HPP

#include <brfc/String.hpp>

#include <brfc/sql/Selectable.hpp>

namespace brfc {
namespace sql {

/**
 * @brief an alias for a Selectable
 *
 * aliased AS alias
 */
class Alias : public Selectable {
  public:
    /**
     * @brief construct as a shared_ptr
     * @sa Alias()
     */
    static AliasPtr create(SelectablePtr aliased,
                           const String& alias) {
        return AliasPtr(new Alias(aliased, alias));
    }
    
    /**
     * @brief the aliased Selectable
     */
    SelectablePtr aliased() const {
        return aliased_;
    }
    
    /**
     * @brief the alias
     */
    const String& alias() const {
        return alias_;
    }
    
    /**
     * @brief the alias
     */
    virtual String name() const {
        return alias_;
    }
    
    /**
     * @brief columns of this alias
     * 
     * the returned columns are proxies for the columns on the
     * aliased Selectable
     */
    virtual std::vector<ColumnPtr> columns() const;

  protected:
    /**
     * @brief constructor
     * @param aliased the selectable this alias is for
     * @param alias name of the alias
     */
    Alias(SelectablePtr aliased,
          const String& alias)
            : aliased_(aliased)
            , alias_(alias) {
    }

  private:
    SelectablePtr aliased_;
    String alias_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_ALIAS_HPP
