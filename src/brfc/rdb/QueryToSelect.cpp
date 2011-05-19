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

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include <brfc/assert.hpp>
#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/FileQuery.hpp>
#include <brfc/expr/Eval.hpp>
#include <brfc/expr/Listcons.hpp>
#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/Model.hpp>
#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Select.hpp>

namespace brfc {

namespace {

struct attr {
    const AttributeMapper* mapper_;
    sql::Factory xpr_;
    sql::FromClause from_; ///< from clause for the statement
    std::map<std::string, std::string> plain_column_map;

    typedef Expression result_type;
    
    attr(const AttributeMapper* mapper)
            : mapper_(mapper)
            , xpr_()
            , from_()
            , plain_column_map() {
        boost::assign::insert(plain_column_map)
            ("string", "value_str")
            ("int64", "value_int")
            ("double", "value_double")
            ("bool", "value_bool")
            ("date", "value_date")
            ("time", "value_time")
        ;
        reset();
    }

    void reset() {
        // always select from files and join sources
        from_ = sql::FromClause(xpr_.table(m::files::name()));
    }

    void join_sources() {
        Expression sources_t = xpr_.table(m::sources::name());
        if (not from_.contains(sources_t)) {
            from_.join(
                sources_t,
                xpr_.eq(
                    m::files::column("source_id"),
                    m::sources::column("id")
                )
            );
        }
    }

    std::string join_source_kvs(const std::string& key) {
        std::string alias = "src_" + key;
        Expression alias_t = xpr_.alias(xpr_.table("bdb_source_kvs"), alias);

        // join if missing
        if (not from_.contains(alias_t)) {
            from_.outerjoin(
                alias_t,
                xpr_.and_(
                    xpr_.eq(
                        xpr_.column(alias, "source_id"),
                        m::sources::column("id")
                    ),
                    xpr_.eq(
                        xpr_.column(alias, "key"),
                        xpr_.string(key)
                    )
                )
            );
        }
        return alias;
    }

    Expression
    operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 2);

        Expression::const_iterator it = x.begin();
        const std::string& name = (*it).string();
        ++it;
        const std::string& type = (*it).string();
        return column_by_attr(name, type);
    }

   /**
     * @brief turn Attribute into sql::Column
     * @post the expression stack contains equivalent sql::Column
     * 
     * depending on the attribute, column lookup is forwarded to:
     * - source_attr_column(), if the attribute name is 'what/source:*'
     * - specialized_attr_column(), if the attribute is specialized in the
     *   mapper
     * - plain_attr_column() for every other attribute
     */
    Expression
    column_by_attr(const std::string& name, const std::string& type) {
        if (boost::starts_with(name, "what/source:")) {
            return source_attr_column(name);
        } else if (mapper_->has(name)) {
            return specialized_attr_column(name);
        } else {
            return plain_attr_column(name, type);
        }
    }

    /**
     * @brief look up sql::Column for a specialized attribute
     * @return the sql::Column where the attribute is stored
     *
     * look up mapping from the mapper and use Mapping::column as the column
     * for the attribute.
     */
    Expression
    specialized_attr_column(const std::string& name) {
        // currently, specialized attributes are only in bdb_files and
        // bdb_sources. These are already contained in the from clause.
        Mapping mapping = mapper_->mapping(name);
        return xpr_.column(mapping.table, mapping.column);
    }

    /**
     * @brief look up sql::Column for a source attribute
     * @return the sql::Column where the attribute is stored
     *
     * The column is looked up using the $KEY in 'what/source:$KEY':
     * - if $KEY is '_name', the column for the attribute is 'bdb_sources.name'
     * - for every other $KEY, 'bdb_source_kvs' is joined in the from-clause
     *   (if not already joined) using an alias 'src_$KEY' with an additional
     *   join-condition 'bdb_source_kvs.key=$KEY'. The column for the
     *   attribute is 'src_$KEY.value'.
     */
    Expression
    source_attr_column(const std::string& name) {
        const std::string& key = name.substr(name.find_first_of(':') + 1);
        if (key == "_name") {
            join_sources();
            // this attribute can be accessed at sources.name
            return m::sources::column("name");
        } else {
            join_sources();
            const std::string& alias = join_source_kvs(key);
            // this attribute can be accessed at src_$KEY.value
            return xpr_.column(alias, "value");
        }
    }

    /**
     * @brief look up sql::Column for a plain attribute
     * @return the sql::Column where the attribute is stored
     *
     * Call join_attrs() to join 'attrs' and 'pnode1'. Alias
     * 'bdb_attribute_values' for this attribute as '$NAME_values' and join
     * it in the from clause (if not already joined).
     *
     * The column for the attribute is '$NAME_values.value_$TYPE'.
     */
    Expression
    plain_attr_column(std::string name, const std::string& type) {
        std::list<std::string> path;
        boost::split(path, name, boost::is_any_of("/"),
                     boost::token_compress_on);
        std::string attrname = path.back();
        path.pop_back();
        std::string groupname;
        if (not path.empty()) {
            groupname = path.back();
            path.pop_back();
        }
        
        // alias the table (this attribute is always searched on this alias)
        boost::erase_all(name, "/");
        std::string alias = name + "_values";
        Expression alias_t = xpr_.alias(xpr_.table(m::attrvals::name()), alias);
        
        // join this table-alias if not already joined
        if (not from_.contains(alias_t)) {
            // join attribute layer to files
            std::string l0_alias = name + "_l0";
            from_.outerjoin(
                xpr_.alias(xpr_.table(m::nodes::name()), l0_alias),
                xpr_.and_(
                    xpr_.eq(
                        xpr_.column(l0_alias, "file_id"),
                        m::files::column("id")
                    ),
                    xpr_.eq(
                        xpr_.column(l0_alias, "name"),
                        xpr_.string(attrname)
                    )
                )
            );
            // join group layer to attribute layer
            std::string l1_alias = name + "_l1";
            from_.outerjoin(
                xpr_.alias(xpr_.table(m::nodes::name()), l1_alias),
                xpr_.and_(
                    xpr_.eq(
                        xpr_.column(l1_alias, "id"),
                        xpr_.column(l0_alias, "parent_id")
                    ),
                    xpr_.eq(
                        xpr_.column(l1_alias, "name"),
                        xpr_.string(groupname)
                    )
                )
            );
            // join attribute values to attribute layer
            from_.outerjoin(
                alias_t,
                xpr_.eq(
                    xpr_.column(alias, "node_id"),
                    xpr_.column(l0_alias, "id")
                )
            );
        }
        return xpr_.column(alias, plain_column_map[type]);
    }
};

struct binop {
    std::string op_;

    typedef Expression result_type;

    explicit binop(const std::string& op) : op_(op) { }

    Expression operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 2);
        return Listcons().symbol(op_).extend(x).get();
    }
};

struct unaryop {
    std::string op_;

    typedef Expression result_type;

    explicit unaryop(const std::string& op) : op_(op) { }

    Expression operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 1);
        return Listcons().symbol(op_).extend(x).get();
    }
};


struct func {
    std::string name_;

    typedef Expression result_type;

    explicit func(const std::string& name) : name_(name) { }
    
    Expression operator()(const Expression& x) {
        return Listcons().symbol(name_).extend(x).get();
    }
};

struct literal {
    sql::Factory xpr_;

    typedef Expression result_type;

    Expression operator()(const Expression& x) {
        return xpr_.literal(x);
    }
};

struct sql_list {
    sql::Factory xpr_;

    typedef Expression result_type;

    Expression operator()(const Expression& x) {
        return xpr_.list(x);
    }
};

} // namespace anonymous

struct QueryToSelect::Impl {
    typedef boost::function<Expression(const Expression&)> proc_t;
    typedef std::map<std::string, proc_t> procmap_t;

    const AttributeMapper* mapper_;
    procmap_t procs_;
    sql::Factory xpr_;
    sql::Select select_; ///< the select statement

    Eval eval_;
    attr attr_cb;
    literal literal_cb;

    Impl(const AttributeMapper* mapper)
            : mapper_(mapper)
            , procs_()
            , select_()
            , eval_()
            , attr_cb(mapper_)
            , literal_cb() { 
        
        eval_.bind_literal_proc(literal_cb);

        eval_.bind("+",      boost::bind(binop("+"), _1));
        eval_.bind("-",      boost::bind(binop("-"), _1));
        eval_.bind("*",      boost::bind(binop("*"), _1));
        eval_.bind("/",      boost::bind(binop("/"), _1));

        eval_.bind("=",      boost::bind(binop("="), _1));
        eval_.bind("!=",     boost::bind(binop("!="), _1));
        eval_.bind(">",      boost::bind(binop(">"), _1));
        eval_.bind("<",      boost::bind(binop("<"), _1));
        eval_.bind(">=",     boost::bind(binop(">="), _1));
        eval_.bind("<=",     boost::bind(binop("<="), _1));

        eval_.bind("and",    boost::bind(binop("and"), _1));
        eval_.bind("or",     boost::bind(binop("or"), _1));
        eval_.bind("not",    boost::bind(unaryop("not"), _1));

        eval_.bind("like",   boost::bind(binop("like"), _1));
        eval_.bind("in",     boost::bind(binop("in"), _1));

        eval_.bind("list",   boost::bind(sql_list(), _1));
        eval_.bind_special_proc("attr",   boost::bind(boost::ref(attr_cb), _1));

        eval_.bind("count",  boost::bind(func("count"), _1));
        eval_.bind("max",    boost::bind(func("max"), _1));
        eval_.bind("min",    boost::bind(func("min"), _1));
        eval_.bind("sum",    boost::bind(func("sum"), _1));
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
    select_.from(attr_cb.from_);

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
    select_.from(attr_cb.from_);

    return select_;
}

} // namespace brfc
