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

#ifndef BRFC_EXPR_FUNCTION_HPP
#define BRFC_EXPR_FUNCTION_HPP

#include <vector>

#include <string>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

class Function : public Expression {
  public:
    explicit Function(const std::string& name)
            : name_(name)
            , args_() {
    }

    virtual ExpressionPtr clone() const {
        return ExpressionPtr(new Function(*this));
    }

    static FunctionPtr max(const Expression& arg) {
        FunctionPtr f = make_shared<Function>("MAX");
        f->add_arg(arg);
        return f;
    }

    static FunctionPtr min(const Expression& arg) {
        FunctionPtr f = make_shared<Function>("MIN");
        f->add_arg(arg);
        return f;
    }
    
    virtual const std::string& name() const { return name_; }
    
    std::vector<ExpressionPtr> args() const { return args_; }

    void add_arg(const Expression& arg) {
        args_.push_back(arg.clone());
    }
  
  protected:
    Function(const Function& other)
            : name_(other.name_)
            , args_() {
        for (size_t i=0; i < other.args_.size(); ++i) {
            args_.push_back(other.args_.at(i)->clone());
        }
    }
   
  private:
    std::string name_;
    std::vector<ExpressionPtr> args_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_FUNCTION_HPP
