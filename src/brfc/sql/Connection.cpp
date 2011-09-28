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

#include <brfc/sql/Connection.hpp>

#include <memory>
#include <sstream>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/sql/Compiler.hpp>
#include <brfc/sql/Dialect.hpp>
#include <brfc/sql/Insert.hpp>
#include <brfc/sql/Result.hpp>
#include <brfc/sql/Select.hpp>

namespace brfc {
namespace sql {

Connection::~Connection() {

}

void
Connection::close() {
    do_close();
}

void
Connection::begin() {
    do_begin();
}

void
Connection::rollback() {
    do_rollback();
}

void
Connection::commit() {
    do_commit();
}

Result*
Connection::execute(const Insert& stmt) {
    return execute(compiler().compile(stmt.expression()));
}

Result*
Connection::execute(const Select& stmt) {
    return execute(compiler().compile(stmt.expression()));
}

Result*
Connection::execute(const Expression& stmt, const BindMap_t& binds) {
    return execute(replace_binds(stmt, binds));
}

Result*
Connection::execute(const Expression& stmt) {
    BindMap_t binds;
    return execute(replace_binds(stmt, binds));
}

Result*
Connection::execute(const std::string& stmt) {
    std::auto_ptr<Result> result;
    if (not in_transaction()) {
        try {
            begin();
            result.reset(do_execute(stmt));
            commit();
        } catch (const std::runtime_error& e) {
            rollback();
            throw db_error(e.what());
        }
    } else {
        try {
            result.reset(do_execute(stmt));
        } catch (const std::runtime_error& e) {
            throw db_error(e.what());
        }
    }
    return result.release();
}

namespace {

std::string
get_bind(const Dialect& d, const Expression& x, const Connection::BindMap_t& binds) {
    BRFC_ASSERT(x.is_list());
    Expression::const_iterator it = x.begin();
    BRFC_ASSERT(it->is_symbol());
    BRFC_ASSERT(it->symbol() == "bind");
    ++it;
    BRFC_ASSERT(it != x.end());
    BRFC_ASSERT(it->is_string());
    
    Connection::BindMap_t::const_iterator b_it = binds.find(it->string());
    const Expression& e = b_it != binds.end() ? b_it->second  : Expression();

    return d.literal_to_string(e);
}

} // namepsace anonymous

std::string
Connection::replace_binds(const Expression& stmt,
                          const BindMap_t& binds) const {
    BRFC_ASSERT(stmt.is_list());
    
    std::stringstream ss;
    Expression::const_iterator it = stmt.begin();
    for ( ; it != stmt.end(); ++it) {
        if (it->is_string()) {
            ss << it->string();
        } else if (it->is_list()) {
            ss << get_bind(dialect(), *it, binds);
        } else {
            BRFC_ASSERT(false);
        }
    }
    return ss.str();
}

long long
Connection::store_large_object(const std::string& path) {
    return do_store_large_object(path);
}

void
Connection::large_object_to_file(long long id, const std::string& path) {
    return do_large_object_to_file(id, path);
}

} // namespace sql
} // namespace brfc
