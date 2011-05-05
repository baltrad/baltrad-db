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

#ifndef BRFC_SQL_FACTORY_HPP
#define BRFC_SQL_FACTORY_HPP

#include <string>

namespace brfc {

namespace expr {
    class Expression;
}

class Date;
class DateTime;
class Time;
class Variant;

namespace sql {

/**
 * @brief factory for constructing SQL expression elements
 */
class Factory {
  public:
    /**
     * @brief construct Literal from Variant
     */
    expr::Expression literal(const expr::Expression& value) const;

    /**
     * @brief construct Literal containing a string
     */
    expr::Expression string(const std::string& value) const;

    /**
     * @brief construct Literal containing a string
     */
    expr::Expression string(const char* value) const;

    /**
     * @brief construct Literal containing a 64-bit integer
     */
    expr::Expression int64_(long long value) const;

    /**
     * @brief construct Literal containing a double precision float
     */
    expr::Expression double_(double value) const;

    /**
     * @brief construct Literal containing a date
     */
    expr::Expression date(const Date& date) const;

    /**
     * @brief construct Literal containing a time
     */
    expr::Expression time(const Time& time) const;

    /**
     * @brief construct Literal containing a datetime
     */
    expr::Expression datetime(const DateTime& datetime) const;

    /**
     * @brief construct Literal containing a bool
     */
    expr::Expression bool_(bool value) const;

    expr::Expression bind(const std::string& name) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    expr::Expression ne(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief lhs == rhs
     */
    expr::Expression eq(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief lhs > rhs
     */
    expr::Expression gt(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief lhs < rhs
     */
    expr::Expression lt(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief lhs <= rhs
     */
    expr::Expression le(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief lhs >= rhs
     */
    expr::Expression ge(const expr::Expression& lhs, const expr::Expression& rhs) const;
    
    /**
     * @brief low <= sql => high
     */
    expr::Expression between(const expr::Expression& sql,
                              const expr::Expression& low,
                              const expr::Expression& high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    expr::Expression and_(const expr::Expression& lhs, const expr::Expression& rhs) const;
    
    /**
     * @brief lhs OR rhs
     */
    expr::Expression or_(const expr::Expression& lhs, const expr::Expression& rhs) const;

    /**
     * @brief NOT exp
     */
    expr::Expression not_(const expr::Expression& exp) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief lhs + rhs
     */
    expr::Expression add(const expr::Expression& lhs, const expr::Expression& rhs) const;

    expr::Expression column(const std::string& table,
                            const std::string& column) const;

    expr::Expression label(const expr::Expression& x, const std::string& label) const;

    expr::Expression alias(const expr::Expression& x, const std::string& alias) const;

    expr::Expression table(const std::string& table) const;

    expr::Expression min(const expr::Expression& x) const;
    expr::Expression max(const expr::Expression& x) const;
    expr::Expression sum(const expr::Expression& x) const;
    expr::Expression count(const expr::Expression& x) const;


    expr::Expression binary_op(const std::string& op,
                               const expr::Expression& lhs,
                               const expr::Expression& rhs) const;

    expr::Expression unary_op(const std::string& op,
                              const expr::Expression& arg) const;
};


}
}

#endif // BRFC_SQL_FACTORY_HPP
