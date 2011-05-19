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

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <brfc/expr/Expression.hpp>

namespace brfc {

namespace {

Expression
default_literal_proc(const Expression& x) {
    return x; 
}

} // namespace anonymous

Eval::Eval()
        : procs_()
        , proc_literal_(boost::bind(&default_literal_proc, _1)) {
}

void
Eval::bind(const std::string& symbol, proc_t proc) {
    procs_[symbol] = proc;
}

void
Eval::bind_special_proc(const std::string& symbol, proc_t proc) {
    special_procs_[symbol] = proc;
}

void
Eval::bind_literal_proc(proc_t proc) {
    proc_literal_ = proc;
}

Expression
Eval::operator()(const Expression& x) {
    if (x.is_list()) {
        Expression::const_iterator it = x.begin(); 
        if (it != x.end() and it->is_symbol()) {
            const std::string& symbol = it->symbol();
            ++it;
            return call_proc(symbol, Expression(it, x.end()));
        } else {
            throw std::logic_error("invalid list to eval: " + 
                                   boost::lexical_cast<std::string>(x));
        }
    } else {
        return proc_literal_(x);
    }
}

Expression
Eval::call_proc(const std::string& symbol, const Expression& x) {
    procmap_t::const_iterator it = special_procs_.find(symbol);
    if (it != special_procs_.end()) {
        return it->second(x);
    }
    it = procs_.find(symbol);
    if (it != procs_.end()) {
        return it->second(eval_list(x));
    }
    throw std::logic_error("no proc for symbol: " + symbol);
}

Expression
Eval::eval_list(const Expression& x) {
    Expression newx;
    Expression::const_iterator it = x.begin(); 
    for ( ; it != x.end(); ++it) {
        newx.push_back(operator()(*it));
    }
    return newx;
}

} // namespace brfc
