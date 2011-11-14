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

def add(lhs, rhs):
    return ["+", lhs, rhs]

def sub(lhs, rhs):
    return ["-", lhs, rhs]

def mul(lhs, rhs):
    return ["*", lhs, rhs]

def div(lhs, rhs):
    return ["/", lhs, rhs]

def and_(lhs, rhs):
    return ["and", lhs, rhs]

def or_(lhs, rhs):
    return ["or", lhs, rhs]

def not_(xpr):
    return ["not", xpr]

def attribute(name, type_):
    return ["attr", name, type_]

def literal(lit):
    if isinstance(lit, list):
        return ["list"] + lit
    elif isinstance(lit, datetime.datetime):
        return ["datetime", lit.year, lit.month, lit.day,
                            lit.hour, lit.minute, lit.second]
    elif isinstance(lit, datetime.date):
        return ["date", lit.year, lit.month, lit.day]
    elif isinstance(lit, datetime.time):
        return ["time", lit.hour, lit.minute, lit.second]
    return lit

def eq(lhs, rhs):
    return ["=", lhs, rhs]

def le(lhs, rhs):
    return ["<=", lhs, rhs]

def lt(lhs, rhs):
    return ["<", lhs, rhs]

def gt(lhs, rhs):
    return [">", lhs, rhs]

def ge(lhs, rhs):
    return ["<", lhs, rhs]

def between(xpr, low, high):
    return and_(le(low, xpr), le(xpr, high))

def like(xpr, pattern):
    return ["like", xpr, pattern]

def asc(xpr):
    return ["asc", xpr]

def desc(xpr):
    return ["desc", xpr]

def in_(lhs, rhs):
    return ["in", lhs, rhs]

def min(xpr):
    return ["min", xpr]

def max(xpr):
    return ["max", xpr]

def sum(xpr):
    return ["sum", xpr]

def count(xpr):
    return ["count", xpr]
