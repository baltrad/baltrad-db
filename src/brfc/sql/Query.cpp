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

#include <string>

#include <boost/foreach.hpp>
#include <boost/xpressive/xpressive_static.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/sql/Dialect.hpp>

namespace brfc {
namespace sql {

Query::Query(const String& stmt, const BindMap& binds)
        : stmt_(stmt)
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

namespace {

struct formatter {
    typedef std::map<std::string, std::string> rpl_map;

    rpl_map map;

    formatter(const BindMap& binds, const Dialect& dialect)
            : map() {
        BOOST_FOREACH(const BindMap::value_type& kv, binds) {
            map[kv.first.to_std_string()] = dialect.variant_to_string(kv.second).to_std_string();
        }
    }
    
    const std::string& operator()(const boost::xpressive::smatch& match) const {
        rpl_map::const_iterator iter = map.find(match[0]);
        if (iter == map.end())
            throw lookup_error(match[0]);
        return iter->second;
    }

};

} // namespace anonymous

String
Query::replace_binds(const Dialect& dialect) const {
    std::string std_stmt = statement().to_std_string();

    formatter fmt(binds_, dialect);

    using namespace boost::xpressive;
    static const sregex re = ~after(as_xpr(':') | '\\') >> ":" >> +(alpha | '_') >> *(alnum | '_');

    std::string replaced = regex_replace(std_stmt, re, fmt); 
    return String(replaced);
}

} // namespace sql
} // namespace brfc
