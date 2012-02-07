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

import datetime

class Symbol(object):
    """Symbol expression
    """
    def __init__(self, value):
        self.value = value
    
    def __eq__(self, other):
        if isinstance(other, Symbol):
            return self.value == other.value
        return False
    
    def __repr__(self):
        return "Symbol('" + self.value + "')"

def binary_operator(op, lhs, rhs):
    return [symbol(op), lhs, rhs]

def add(lhs, rhs):
    return binary_operator("+", lhs, rhs)

def sub(lhs, rhs):
    return binary_operator("-", lhs, rhs)

def mul(lhs, rhs):
    return binary_operator("*", lhs, rhs)

def div(lhs, rhs):
    return binary_operator("/", lhs, rhs)

def and_(lhs, rhs):
    return binary_operator("and", lhs, rhs)

def or_(lhs, rhs):
    return binary_operator("or", lhs, rhs)

def not_(xpr):
    return [symbol("not"), xpr]

def attribute(name, type_):
    return [symbol("attr"), name, type_]

def literal(lit):
    if isinstance(lit, datetime.datetime):
        return [symbol("datetime"),
                lit.year, lit.month, lit.day,
                lit.hour, lit.minute, lit.second]
    elif isinstance(lit, datetime.date):
        return [symbol("date"), lit.year, lit.month, lit.day]
    elif isinstance(lit, datetime.time):
        return [symbol("time"), lit.hour, lit.minute, lit.second]
    return lit

def symbol(value):
    return Symbol(value)

def eq(lhs, rhs):
    return binary_operator("=", lhs, rhs)

def le(lhs, rhs):
    return binary_operator("<=", lhs, rhs)

def lt(lhs, rhs):
    return binary_operator("<", lhs, rhs)

def gt(lhs, rhs):
    return binary_operator(">", lhs, rhs)

def ge(lhs, rhs):
    return binary_operator("<", lhs, rhs)

def between(xpr, low, high):
    return and_(le(low, xpr), lt(xpr, high))

def like(xpr, pattern):
    return binary_operator("like", xpr, pattern)

def asc(xpr):
    return [symbol("asc"), xpr]

def desc(xpr):
    return [symbol("desc"), xpr]

def in_(lhs, rhs):
    return binary_operator("in", lhs, rhs)

def min(xpr):
    return [symbol("min"), xpr]

def max(xpr):
    return [symbol("max"), xpr]

def sum(xpr):
    return [symbol("sum"), xpr]

def count(xpr):
    return [symbol("count"), xpr]

def unwrap_json(xpr):
    """unwrap an expression parsed from JSON
    
    turn arrays beginning with ["symbol", ...] to :class:`Symbol` objects and
    arrays beginning with ["list", ...] to :class:`list` objects
    """
    if isinstance(xpr, list):
        if xpr[0] == "symbol" and len(xpr) == 2:
            return Symbol(xpr[1])
        if xpr[0] == "list":
            return [unwrap_json(child_xpr) for child_xpr in xpr[1:]]
        else:
            raise ValueError("unhandled type at xpr[0]: %s" % xpr[0])
    else:
        return xpr

def wrap_json(xpr):
    """wrap an expression to JSON

    turn :class:`Symbol` objects to ["symbol", ...] and lists to ["list", ...]
    """
    if isinstance(xpr, Symbol):
        return ["symbol", xpr.value]
    elif isinstance(xpr, list):
        wrapped = ["list"]
        for child_xpr in xpr:
            wrapped.append(wrap_json(child_xpr))
        return wrapped
    else:
        return xpr

class EvaluationError(RuntimeError):
    """Expression evaluation failure
    """
    pass

class Evaluator(object):
    """Expression evaluator
    """
    def __init__(self):
        self._procedures = {}
    
    def add_procedure(self, name, callback):
        """add a procedure to this evaluator

        :param name: name of the procedure
        :param callback: a callable object
        """
        self._procedures[name] = callback
    
    def get_procedure(self, name):
        """access a procedure defined in this evaluator

        :param name: procedure name
        :return: the callable associated with `name`
        :raise: :class:`LookupError` if procedure is not found by `name`
        """
        return self._procedures[name]

    def evaluate(self, xpr):
        """evaluate an expression

        :param xpr: the expression to evaluate
        :return: evaluation result
        :raise: :class:`EvaluationError` when the evaluation fails

        see :ref:`doc-expr-eval` for the evaluation rules
        """
        if isinstance(xpr, Symbol):
            try:
                return self.get_procedure(xpr.value)
            except LookupError:
                raise EvaluationError("No procedure found: %s" % xpr.value)
        elif not isinstance(xpr, list):
            return xpr
        else:
            if len(xpr) == 0:
                return xpr
            xpr = [self.evaluate(child_xpr) for child_xpr in xpr]
            if callable(xpr[0]):
                proc = xpr.pop(0)
                try:
                    return proc(*xpr)
                except TypeError, e:
                    raise EvaluationError(str(e))
            else:
                return xpr
