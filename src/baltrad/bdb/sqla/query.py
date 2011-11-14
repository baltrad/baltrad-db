# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import operator

from sqlalchemy import sql

from . import schema

class ExprToSql(object):
    def __init__(self):
        self.where_clause = sql.literal(True)
        self.from_clause = schema.files

        self._handlers = {
            "+":   operator.add,
            "-":   operator.sub,
            "*":   operator.mul,
            "/":   operator.div,
            
            "=":   operator.eq,
            "!=":  operator.ne,
            "<":   operator.lt,
            ">":   operator.gt,
            ">=":  operator.ge,
            "<=":  operator.le,
    
            "and": operator.and_,
            "or":  operator.or_,
            "not": operator.inv,
           
            "like": self.like,
            "in": lambda lhs, rhs: lhs.in_(rhs),
            "list": list,
            "attr": self.get_attr_column,

            "asc": lambda col: col.asc(),
            "desc": lambda col: col.desc(),
            
            "list": lambda *args: list(args),
            "lit": sql.literal,
            "count": sql.func.count,
            "max": sql.func.max,
            "min": sql.func.min,
            "sum": sql.func.sum,
        }
        
        self._mapper = {
            "file:uuid": schema.files.c.uuid,
            "file:stored_date": schema.files.c.stored_date,
            "file:stored_time": schema.files.c.stored_time,
            "what/object": schema.files.c.what_object,
            "what/date": schema.files.c.what_date,
            "what/time": schema.files.c.what_time,
        }

        self._value_tables = { }
    
    def like(self, lhs, rhs):
        return lhs.like(rhs.replace("*", "%"))
    
    def get_attr_column(self, name, type_):
        if name.startswith("what/source:"):
            column = self.get_source_attr_column(name)
        elif (name in self._mapper):
            column = self.get_specialized_attr_column(name)
        else:
            column = self.get_plain_attr_column(name, type_)
        return column
    
    def get_source_attr_column(self, name):
        key = name.rsplit(":", 1)[-1]
        if not self.from_clause_contains(schema.sources):
            self.from_clause = self.from_clause.join(schema.sources)

        if key == "_name":
            return schema.sources.c.name
        else:
            alias_t = schema.source_kvs.alias("src_" + key)
            if not self.from_clause_contains(alias_t):
                self.from_clause = self.from_clause.outerjoin(
                    alias_t,
                    onclause=sql.and_(
                        alias_t.c.source_id==schema.sources.c.id,
                        alias_t.c.key==key
                    )
                )
            return alias_t.c.value
        
    def get_specialized_attr_column(self, name):
        return self._mapper[name]
    
    def get_plain_attr_column(self, name, type_):
        elems = name.split("/")
        attrname = elems.pop()
        
        name = name.replace("/", "_")
        value_alias_t = self._value_tables.setdefault(
            name + "_values",
            schema.attribute_values.alias(name + "_values")
        )

        if not self.from_clause_contains(value_alias_t):
            l0_node_alias = schema.nodes.alias(name + "_l0")
            self.from_clause = self.from_clause.outerjoin(
                l0_node_alias,
                onclause=sql.and_(
                    l0_node_alias.c.file_id==schema.files.c.id,
                    l0_node_alias.c.name==attrname
                )
            )
            l1_node_alias = schema.nodes.alias(name + "_l1")
            self.from_clause = self.from_clause.outerjoin(
                l1_node_alias,
                onclause=sql.and_(
                    l1_node_alias.c.id==l0_node_alias.c.parent_id,
                    l1_node_alias.c.name==elems[-1]
                )
            )
            self.from_clause = self.from_clause.outerjoin(
                value_alias_t,
                onclause=value_alias_t.c.node_id==l0_node_alias.c.id
            )
        return getattr(value_alias_t.c, "value_" + type_)
    
    def from_clause_contains(self, table):
        tables = sql.util.find_tables(self.from_clause, include_aliases=True)
        return table in tables

    def eval_(self, expr):
        if not isinstance(expr, list):
            return expr
        else:
            try:
                proc = self.get_proc(expr[0])
            except LookupError:
                raise RuntimeError("no procedure for expression: %r" % expr)
            expr = [self.eval_(x) for x in expr[1:]]
            return proc(*expr)
            
    def __call__(self, expr):
        return self.eval_(expr)
    
    def get_proc(self, op):
        return self._handlers[op]

def _order_clause(clause):
    if clause.modifier == sql.operators.desc_op:
        return sql.func.max(clause.element).desc()
    elif clause.modifier == sql.operators.asc_op:
        return sql.func.min(clause.element).asc()
    else:
        raise RuntimeError("order by clause missing modifier")
    
def transform_file_query(query):
    evaluator = ExprToSql()

    where_clause = evaluator(query.filter)
    order_by_clauses = query.order
    group_by_clauses = []
    distinct = False
    select_columns = [schema.files.c.uuid]

    if order_by_clauses:
        order_by_clauses = [evaluator(xpr) for xpr in order_by_clauses]
        order_by_clauses = [_order_clause(clause) for clause in order_by_clauses]
        group_by_clauses.append(schema.files.c.uuid)
    else:
        stored_timestamp = (schema.files.c.stored_date + schema.files.c.stored_time).label("stored_timestamp")
        select_columns.append(stored_timestamp)
        order_by_clauses.append(stored_timestamp.asc())
        distinct = True

    return sql.select(
        select_columns,
        where_clause,
        from_obj=evaluator.from_clause,
        distinct=distinct,
        order_by=order_by_clauses,
        group_by=group_by_clauses,
        limit=query.limit,
        offset=query.skip,
    )

def transform_attribute_query(query):
    evaluator = ExprToSql()

    select_columns=[]

    for key, xpr in query.fetch.iteritems():
        attr_column = evaluator(xpr)
        select_columns.append(attr_column.label(key))

    where_clause = evaluator(query.filter)
    order_by_clauses = [evaluator(xpr) for xpr in query.order]
    group_by_clauses = [evaluator(xpr) for xpr in query.group]

    return sql.select(
        select_columns,
        where_clause,
        from_obj=evaluator.from_clause,
        distinct=query.distinct,
        order_by=order_by_clauses,
        group_by=group_by_clauses,
        limit=query.limit,
    )
