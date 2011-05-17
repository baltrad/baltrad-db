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

#include <brfc/oh5/FileMatcher.hpp>

#include <boost/bind.hpp>

#include <brfc/expr/proc/arithmetic.hpp>
#include <brfc/expr/proc/binary_list.hpp>
#include <brfc/expr/proc/comparison.hpp>
#include <brfc/expr/proc/in.hpp>
#include <brfc/expr/proc/like.hpp>
#include <brfc/expr/proc/list_filter.hpp>
#include <brfc/expr/proc/logic.hpp>

namespace brfc {
namespace oh5 {

struct list_in {
    typedef Expression result_type;

    Expression operator()(const Expression& args) const {
        BRFC_ASSERT(args.size() == 2);
        Expression::const_iterator it = args.begin();
        const Expression& lhs = *it;
        ++it;
        const Expression& rhs = *it;
        BRFC_ASSERT(rhs.is_list());

        if (lhs.is_list()) {
            for (it = lhs.begin(); it != lhs.end(); ++it) {
                if (rhs.contains(*it))
                    return Expression(true);
            }
            return Expression(false);
        } else {
            return Expression(rhs.contains(lhs));
        }
    }
};

FileMatcher::FileMatcher()
        : attr_() 
        , eval_() {
    using ::brfc::proc::binary_list;
    using ::brfc::proc::list_filter;

    eval_.bind("+", boost::bind(binary_list(proc::add()), _1));
    eval_.bind("-", boost::bind(binary_list(proc::sub()), _1));
    eval_.bind("*", boost::bind(binary_list(proc::mul()), _1));
    eval_.bind("/", boost::bind(binary_list(proc::div()), _1));
    eval_.bind("=", boost::bind(list_filter(proc::eq()), _1));
    eval_.bind("!=", boost::bind(list_filter(proc::ne()), _1));
    eval_.bind(">", boost::bind(list_filter(proc::gt()), _1));
    eval_.bind("<", boost::bind(list_filter(proc::lt()), _1));
    eval_.bind(">=", boost::bind(list_filter(proc::ge()), _1));
    eval_.bind("<=", boost::bind(list_filter(proc::le()), _1));
    eval_.bind("and", boost::bind(proc::and_(), _1));
    eval_.bind("or", boost::bind(proc::or_(), _1));
    eval_.bind("not", boost::bind(proc::not_(), _1));

    eval_.bind("like", boost::bind(list_filter(proc::like()), _1));
    eval_.bind("in", boost::bind(list_in(), _1));

    eval_.bind("attr", boost::bind(boost::ref(attr_), _1));
}

bool
FileMatcher::match(const File& file, const Expression& x) {
    attr_.file(&file);
    const Expression& result = eval_(x);
    attr_.file(0);
    return result;
}

} // namespace oh5
} // namespace brfc
