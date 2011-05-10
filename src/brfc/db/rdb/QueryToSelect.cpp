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

#include <brfc/db/rdb/QueryToSelect.hpp>

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
#include <brfc/db/rdb/AttributeMapper.hpp>
#include <brfc/db/rdb/Model.hpp>

#include <brfc/expr/Listcons.hpp>

#include <brfc/sql/Factory.hpp>
#include <brfc/sql/Select.hpp>

namespace brfc {
namespace db {
namespace rdb {

using expr::Expression;

namespace {

struct attr {

    const AttributeMapper* mapper_;
    sql::Factory xpr_;
    sql::FromClause from_; ///< from clause for the statement
    std::map<std::string, std::string> plain_column_map;
    
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
        from_.join(
            xpr_.table(m::sources::name()),
            xpr_.eq(
                m::files::column("source_id"),
                m::sources::column("id")
            )
        );
    }

    Expression
    operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 3);

        Expression::const_iterator it = x.begin();
        ++it; // skip symbol
        const std::string& name = (*it).string();
        ++it;
        const std::string& type = (*it).string();
        return column_by_attr(name, type);
    }

   /**
     * @brief turn expr::Attribute into sql::Column
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
            // sources is joined by default
            // this attribute can be accessed at sources.name
            return m::sources::column("name");
        } else {
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
    typedef boost::function<Expression(const Expression&)> eval_t;

    eval_t eval;
    std::map<std::string, std::string> opmap;

    explicit binop(eval_t eval_)
            : eval(eval_)
            , opmap() {
        boost::assign::insert(opmap)
            ("!=", "!=")
            ("=", "=")
            (">", ">")
            ("<", "<")
            ("<=", "<=")
            (">=", ">=")
            ("like", "like")
            ("in", "in")
            ("and", "and")
            ("or", "or")
            ("+", "+")
            ("-", "-")
            ("*", "*")
            ("/", "/")
        ;
    }

    Expression operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 3);

        Expression::const_iterator it = x.begin();
        std::string op = opmap[it->symbol()];
        ++it;
        Expression lhs = eval(*it);
        ++it;
        Expression rhs = eval(*it);
        return expr::Listcons().symbol(op).append(lhs).append(rhs).get();
    }
};

struct unaryop {
    typedef boost::function<Expression(const Expression&)> eval_t;

    eval_t eval;
    std::map<std::string, std::string> opmap;

    explicit unaryop(eval_t eval_)
            : eval(eval_)
            , opmap() {
        boost::assign::insert(opmap)
            ("not", "not")
        ;
    }

    Expression operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() == 2);

        Expression::const_iterator it = x.begin();
        std::string op = opmap[it->symbol()];
        ++it;
        Expression arg = eval(*it);
        return expr::Listcons().symbol(op).append(arg).get();
    }
};


struct func {
    typedef boost::function<Expression(const Expression&)> eval_t;

    eval_t eval;

    explicit func(eval_t eval_) : eval(eval_) { }
    
    Expression operator()(const Expression& x) {
        BRFC_ASSERT(x.is_list());
        BRFC_ASSERT(x.size() >= 1);

        Expression f;
        f.push_back(Expression::symbol(x.front().symbol()));
        Expression::const_iterator it = x.begin();
        ++it; // skip symbol
        for ( ; it != x.end(); ++it) {
            f.push_back(eval(*it));
        }
        return f;
    }
};

struct literal {
    sql::Factory xpr_;

    Expression operator()(const Expression& x) {
        return xpr_.literal(x);
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

    attr attr_cb;
    literal literal_cb;

    Impl(const AttributeMapper* mapper)
            : mapper_(mapper)
            , procs_()
            , select_()
            , attr_cb(mapper_)
            , literal_cb() { 
        
        proc_t eval_cb = boost::bind(&QueryToSelect::Impl::eval, this, _1);

        boost::assign::insert(procs_)
            ("+",      binop(eval_cb))
            ("-",      binop(eval_cb))
            ("*",      binop(eval_cb))
            ("/",      binop(eval_cb))

            ("!=",     binop(eval_cb))
            ("=",      binop(eval_cb))
            (">",      binop(eval_cb))
            ("<",      binop(eval_cb))
            (">=",     binop(eval_cb))
            ("<=",     binop(eval_cb))

            ("and",    binop(eval_cb))
            ("or",     binop(eval_cb))
            ("not",    unaryop(eval_cb))

            ("like",   binop(eval_cb))
            ("in",     binop(eval_cb))

            ("attr",   attr_cb)

            ("count",  func(eval_cb))
            ("max",    func(eval_cb))
            ("min",    func(eval_cb))
            ("sum",    func(eval_cb))
        ;
    }

    /**
     * @brief reset to default state
     *
     * set from clause to 'bdb_files' joined to 'bdb_sources', clear the
     * expression stack
     */
    void reset();

    Expression eval(const Expression& x) {
        if (x.is_list() and not x.empty() and x.front().is_symbol()) {
            const std::string& symbol = x.front().symbol();
            if (symbol == "attr")
                return attr_cb(x);
            else
                return procs_[symbol](x);
        } else {
            return literal_cb(x);
        }
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
    BOOST_FOREACH(const expr::Expression& x, query.group()) {
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

} // namespace rdb
} // namespace db
} // namespace brfc
