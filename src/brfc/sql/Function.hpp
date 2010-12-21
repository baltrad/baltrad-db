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

#ifndef BRFC_SQL_FUNCTION_HPP
#define BRFC_SQL_FUNCTION_HPP

#include <vector>

#include <string>

#include <brfc/sql/Expression.hpp>

namespace brfc {
namespace sql {

class Function : public Expression {
  public:
    static FunctionPtr create(const std::string& name) {
        return FunctionPtr(new Function(name));
    }

    static FunctionPtr min(const ExpressionPtr& arg) {
        FunctionPtr f = Function::create("MIN");
        f->add_arg(arg);
        return f;
    }

    static FunctionPtr max(const ExpressionPtr& arg) {
        FunctionPtr f = Function::create("MAX");
        f->add_arg(arg);
        return f;
    }
    
    virtual const std::string& name() const { return name_; }
    
    std::vector<ExpressionPtr> args() const { return args_; }

    void add_arg(ExpressionPtr arg) {
        args_.push_back(arg);
    }
   
  protected:
    explicit Function(const std::string& name)
            : name_(name)
            , args_() {
    }

  private:
    std::string name_;
    std::vector<ExpressionPtr> args_;
};

} // namespace sql
} // namespace brfc

#endif // BRFC_SQL_FUNCTION_HPP
