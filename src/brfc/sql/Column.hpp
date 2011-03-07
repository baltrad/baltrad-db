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

#ifndef BRFC_SQL_COLUMN_HPP
#define BRFC_SQL_COLUMN_HPP

#include <string>

#include <brfc/sql/Expression.hpp>

#include <brfc/sql/fwd.hpp>

namespace brfc {
namespace sql {

/**
 * @brief Column on any Selectable
 *
 * a column might also be a proxy for another column, meaning it is
 * ultimately defined somewhere else, but accessible through (another)
 * Selectable with (another) name.
 */
class Column : public Expression {
  public:
    ColumnPtr shared_from_this() const {
        return static_pointer_cast<Column>(
                const_pointer_cast<Element>(Element::shared_from_this()));
    }

    /**
     * @brief construct as a shared_ptr
     * @sa Column()
     */
    static ColumnPtr create(const std::string& name,
                            SelectablePtr selectable,
                            ColumnPtr parent=ColumnPtr()) {
        return ColumnPtr(new Column(name, selectable, parent));
    }
    
    /**
     * @brief name of this column
     */
    virtual const std::string& name() const { return name_; }
    
    /**
     * @brief the Selectable this column is bound to
     */
    virtual SelectablePtr selectable() const { return selectable_; }

    /**
     * @brief set this column to reference another @c column
     */
    virtual void references(ColumnPtr column);
    
    /**
     * @brief get referenced column
     * @return pointer to referenced column or null pointer
     */
    virtual ColumnPtr references() const {
        if (parent_)
            return parent_->references();
        return references_;
    }
    
    /**
     * @brief proxy this column to another @c selectable
     *
     * proxied column retains its name
     */
    ColumnPtr proxy(SelectablePtr selectable) const;
    
    /**
     * @brief proxy this column to another @c selectable with @c name
     */
    ColumnPtr proxy(const std::string& name, SelectablePtr t) const;
    
    /**
     * @brief test if this column is a proxy of @c col
     * @return true if this columns is a proxy of @c col
     */
    virtual bool is_proxy_of(const Column& col) const;
    
    /**
     * @brief test for column equality
     * @return true if the columns have the same name and same Selectable
     */
    bool operator==(const Column& rhs) const;
  
  protected:
    /**
     * @brief constructor
     * @param name name of this column
     * @param selectable the Selectable this column is bound to
     * @param parent if specified, this column is considered a proxy
     *               of the parent column (default: null)
     */
    Column(const std::string& name,
           SelectablePtr selectable,
           ColumnPtr parent)
            : name_(name)
            , selectable_(selectable)
            , parent_(parent) {
    }

  private:
    std::string name_;
    SelectablePtr selectable_;
    ColumnPtr parent_;
    ColumnPtr references_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_COLUMN_HPP
