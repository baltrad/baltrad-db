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

#ifndef BRFC_EXPR_TABLE_HPP
#define BRFC_EXPR_TABLE_HPP

#include <brfc/expr/Selectable.hpp>

namespace brfc {
namespace expr {

class Table : public Selectable {
  public:
    static TablePtr create(const QString& name) {
        return TablePtr(new Table(name));
    }

    void name(const QString& name) {
        name_ = name;
    }

    virtual QString name() const {
        return name_;
    }

  protected:
    explicit Table(const QString& name)
            : name_(name) {
    }

    virtual void do_accept(Visitor& visitor) {
        visitor.visit(*this);
    }
  
  private:
    QString name_;
};

}
}

#endif // BRFC_EXPR_TABLE_HPP
