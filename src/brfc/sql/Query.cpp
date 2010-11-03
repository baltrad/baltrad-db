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

#include <brfc/sql/Query.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/RegExp.hpp>

#include <brfc/sql/Dialect.hpp>

namespace brfc {
namespace sql {

Query::Query(const String& stmt, const BindMap& binds)
        : stmt_(split_statement(stmt))
        , binds_(binds) {
}

Query::Query(const Query& other)
        : stmt_(other.stmt_)
        , binds_(other.binds_) {
}

Query&
Query::operator=(const Query& rhs) {
    if (this != &rhs) {
        stmt_ = rhs.stmt_;
        binds_ = rhs.binds_;
    }
    return *this;
}

void
Query::statement(const String& stmt) {
    stmt_ = split_statement(stmt);
}

String
Query::statement() const {
    return stmt_.join("");
}

String
Query::replace_binds(const Dialect& dialect) const {
    // this relies on the fact that stmt_ always starts with a "plain" string
    // and then alternates between a bind key and a "plain" string
    String replaced;
    for (size_t i=0; i < stmt_.size(); ++i) {
        const String& stmt_bit = stmt_.at(i);
        if (i % 2) { // bind key
            replaced += dialect.variant_to_string(binds_.get(stmt_bit));
        } else { // "plain" string
            replaced += stmt_bit;
        }
    }
    return replaced;
}

StringList
Query::split_statement(const String& stmt) {
    StringList stmt_bits;
    RegExp bind_re(":[a-zA-Z]+[a-zA-Z0-9_]*");
    int pos = 0, ppos = 0, lpos = 0;
    while (bind_re.index_in(stmt, pos) != -1) {
        ppos = bind_re.pos() - 1;
        /* ':' is precededed by '\' or ':'*/
        if (ppos > -1 and (stmt.char_at(ppos) == '\\' or
                           stmt.char_at(ppos) == ':')) {
            pos = bind_re.pos() + bind_re.matched_length();
            continue;
        }
        stmt_bits.push_back(stmt.substr(lpos, bind_re.pos() - lpos));
        stmt_bits.push_back(bind_re.cap());
        lpos = bind_re.pos() + bind_re.matched_length();
        pos = lpos;
    }
    if (lpos != stmt.length())
        stmt_bits.push_back(stmt.substr(lpos));
    return stmt_bits;
}

} // namespace sql
} // namespace brfc
