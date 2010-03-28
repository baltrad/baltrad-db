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

#ifndef BRFC_EXPR_ALIAS_HPP
#define BRFC_EXPR_ALIAS_HPP

#include <brfc/expr/Selectable.hpp>
#include <brfc/expr/Visitor.hpp>

namespace brfc {
namespace expr {

class Alias : public Selectable {
  public:
    static AliasPtr create(SelectablePtr aliased,
                           const QString& alias) {
        return AliasPtr(new Alias(aliased, alias));
    }

    void aliased(SelectablePtr aliased) {
        aliased_ = aliased;
    }

    SelectablePtr aliased() const {
        return aliased_;
    }

    void alias(const QString& alias) {
        alias_ = alias;
    }

    const QString& alias() const {
        return alias_;
    }

    virtual QString name() const {
        return alias_;
    }

  protected:
    Alias(SelectablePtr aliased,
          const QString& alias)
            : aliased_(aliased)
            , alias_(alias) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }

  private:
    SelectablePtr aliased_;
    QString alias_;
};

}
}

#endif // BRFC_EXPR_ALIAS_HPP
