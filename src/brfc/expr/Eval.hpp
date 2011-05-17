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
#ifndef BRFC_EXPR_EVAL_HPP
#define BRFC_EXPR_EVAL_HPP

#include <map>

#include <boost/function.hpp>

namespace brfc {

class Expression;

class Eval {
  public:
    Eval()
            : procs_() {
    }

    typedef boost::function<Expression(const Expression&)> proc_t;

    void bind(const std::string& symbol, proc_t proc);

    Expression operator()(const Expression& x);
  
  private:
    typedef std::map<std::string, proc_t> procmap_t;
    proc_t proc(const std::string& symbol) const;

    procmap_t procs_;
};

} // namespace brfc

#endif // BRFC_EXPR_EVAL_HPP
