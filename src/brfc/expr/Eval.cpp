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
#include <brfc/expr/Eval.hpp>

#include <stdexcept>

#include <brfc/expr/Expression.hpp>

namespace brfc {
namespace expr {

void
Eval::bind(const std::string& symbol, proc_t proc) {
    procs_[symbol] = proc;
}

Expression
Eval::operator()(const Expression& x) {
    if (x.is_list() and not x.empty()) {
        Expression newx;
        Expression::const_iterator it = x.begin(); 
        for ( ; it != x.end(); ++it) {
            newx.push_back(operator()(*it));
        }
        if (newx.front().is_symbol()) {
            proc_t p = proc(newx.front().symbol());
            newx.pop_front();
            return p(newx);
        }
        return newx;
    } else {
        return x;
    }
}

Eval::proc_t
Eval::proc(const std::string& symbol) const {
    procmap_t::const_iterator it = procs_.find(symbol);
    if (it == procs_.end())
        throw std::logic_error("no proc for symbol: " + symbol);
    return it->second;
}

} // namespace expr
} // namespace brfc
