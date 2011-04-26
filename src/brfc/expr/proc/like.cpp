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

#include <brfc/expr/proc/like.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/xpressive/regex_algorithms.hpp>
#include <boost/xpressive/regex_compiler.hpp>

namespace brfc {
namespace expr {
namespace proc {

Expression
like::operator()(const std::string& str, std::string pattern) const {
    using namespace boost::xpressive;
    using boost::algorithm::replace_all;

    replace_all(pattern, "*", ".*");
    replace_all(pattern, "?", ".{1,1}");
    sregex_compiler compiler;
    sregex re =  compiler.compile(pattern);
    return regex_match(str.begin(), str.end(), re);
}

} // namespace proc
} // namespace expr
} // namespace brfc
