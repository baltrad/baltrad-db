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

#include <brfc/rdb/QueryToSelect.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <brfc/assert.hpp>
#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/expr/Eval.hpp>
#include <brfc/expr/Listcons.hpp>
#include <brfc/expr/proc/make_procedure.hpp>
#include <brfc/rdb/attr_to_column.hpp>
#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Select.hpp>

namespace brfc {

struct QueryToSelect::Impl {
    typedef boost::function<Expression(const Expression&)> proc_t;
    typedef std::map<std::string, proc_t> procmap_t;

    const AttributeMapper* mapper_;
    procmap_t procs_;
    sql::Factory xpr_;
    sql::Select select_; ///< the select statement

    Eval eval_;
    attr_to_column attr_cb;

    Impl(const AttributeMapper* mapper)
            : mapper_(mapper)
            , procs_()
            , select_()
            , eval_()
            , attr_cb(mapper_) {

        using ::brfc::proc::make_procedure;
        
        eval_.bind("+",      boost::bind(make_procedure("+", 2), _1));
        eval_.bind("-",      boost::bind(make_procedure("-", 2), _1));
        eval_.bind("*",      boost::bind(make_procedure("*", 2), _1));
        eval_.bind("/",      boost::bind(make_procedure("/", 2), _1));

        eval_.bind("=",      boost::bind(make_procedure("=", 2), _1));
        eval_.bind("!=",     boost::bind(make_procedure("!=", 2), _1));
        eval_.bind(">",      boost::bind(make_procedure(">", 2), _1));
        eval_.bind("<",      boost::bind(make_procedure("<", 2), _1));
        eval_.bind(">=",     boost::bind(make_procedure(">=", 2), _1));
        eval_.bind("<=",     boost::bind(make_procedure("<=", 2), _1));

        eval_.bind("and",    boost::bind(make_procedure("and", 2), _1));
        eval_.bind("or",     boost::bind(make_procedure("or", 2), _1));
        eval_.bind("not",    boost::bind(make_procedure("not", 1), _1));

        eval_.bind("like",   boost::bind(make_procedure("like", 2), _1));
        eval_.bind("in",     boost::bind(make_procedure("in", 2), _1));

        eval_.bind("list",   boost::bind(make_procedure("list"), _1));
        eval_.bind_special_proc("attr",   boost::bind(boost::ref(attr_cb), _1));

        eval_.bind("count",  boost::bind(make_procedure("count", 1), _1));
        eval_.bind("max",    boost::bind(make_procedure("max", 1), _1));
        eval_.bind("min",    boost::bind(make_procedure("min", 1), _1));
        eval_.bind("sum",    boost::bind(make_procedure("sum", 1), _1));
    }

    /**
     * @brief reset to default state
     *
     * set from clause to 'bdb_files' joined to 'bdb_sources', clear the
     * expression stack
     */
    void reset();

    Expression eval(const Expression& x) {
        return eval_(x);
    }

    sql::Select transform(const FileQuery& query);
    sql::Select transform(const AttributeQuery& query);
};

QueryToSelect::QueryToSelect(const AttributeMapper* mapper)
        : impl_(new Impl(mapper)) {

}

QueryToSelect::~QueryToSelect() {

}

void
QueryToSelect::Impl::reset() {
    select_ = sql::Select();
    attr_cb.reset();
}

sql::Select
QueryToSelect::transform(const FileQuery& query) {
    return impl_->transform(query);
}

sql::Select
QueryToSelect::Impl::transform(const FileQuery& query) {
    reset();

    select_.what(m::files::column("id"));
    select_.limit(query.limit());
    select_.offset(query.skip());

    // replace attributes in where clause with columns
    if (not query.filter().empty()) { // XXX: unnecessary check
        select_.where(eval(query.filter()));
    }

    if (query.order().size() > 0 ) {
        BOOST_FOREACH(FileQuery::OrderPair op, query.order()) {
            Expression order = eval(op.first);
            if (op.second == FileQuery::ASC) {
                order = xpr_.min(order);
            } else {
                order = xpr_.max(order);
            }
            select_.append_order_by(
                order,
                sql::Select::SortDirection(op.second)
            );
        }
        select_.append_group_by(m::files::column("id"));
    } else {
        select_.append_order_by(m::files::column("id"), sql::Select::ASC);
        select_.distinct(true);
    }
    
    // add the built join as from clause
    select_.from(attr_cb.from());

    return select_;
}


sql::Select
QueryToSelect::transform(const AttributeQuery& query) {
    return impl_->transform(query);
}

sql::Select
QueryToSelect::Impl::transform(const AttributeQuery& query) {
    reset();
    
    select_.distinct(query.distinct());
    select_.limit(query.limit());

    // replace attributes with columns
    BOOST_FOREACH(const AttributeQuery::FetchMap::value_type& val, query.fetch()) {
        const Expression& col = eval(val.second);
        select_.what(xpr_.label(col, "l_" + val.first));
    }

    // replace attributes in where clause with columns
    if (not query.filter().empty()) {
        select_.where(eval(query.filter()));
    } 

    // replace attributes in group by
    BOOST_FOREACH(const Expression& x, query.group()) {
        select_.append_group_by(eval(x));
    }

    // replace attributes in order by
    BOOST_FOREACH(AttributeQuery::OrderPair op, query.order()) {
        select_.append_order_by(eval(op.first),
                                 sql::Select::SortDirection(op.second));
    }
    
    // add the built join as from clause
    select_.from(attr_cb.from());

    return select_;
}

} // namespace brfc
