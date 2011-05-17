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
    class Date;
    class DateTime;
    class Expression;
    class Time;
    class Variant;
}

namespace brfc {
namespace sql {

/**
 * @brief factory for constructing SQL expression elements
 */
class Factory {
  public:
    /**
     * @brief construct Literal from Variant
     */
    Expression literal(const Expression& value) const;

    /**
     * @brief construct Literal containing a string
     */
    Expression string(const std::string& value) const;

    /**
     * @brief construct Literal containing a string
     */
    Expression string(const char* value) const;

    /**
     * @brief construct Literal containing a 64-bit integer
     */
    Expression int64_(long long value) const;

    /**
     * @brief construct Literal containing a double precision float
     */
    Expression double_(double value) const;

    /**
     * @brief construct Literal containing a date
     */
    Expression date(const Date& date) const;

    /**
     * @brief construct Literal containing a time
     */
    Expression time(const Time& time) const;

    /**
     * @brief construct Literal containing a datetime
     */
    Expression datetime(const DateTime& datetime) const;

    /**
     * @brief construct Literal containing a bool
     */
    Expression bool_(bool value) const;

    Expression bind(const std::string& name) const;

    /**
     * @name comparison operators
     * @{
     */
    /**
     * @brief lhs != rhs
     */
    Expression ne(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs == rhs
     */
    Expression eq(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs > rhs
     */
    Expression gt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs < rhs
     */
    Expression lt(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs <= rhs
     */
    Expression le(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief lhs >= rhs
     */
    Expression ge(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief low <= sql => high
     */
    Expression between(const Expression& sql,
                              const Expression& low,
                              const Expression& high) const;
    //@}

    /**
     * @name logical operators
     * @{
     */
    /**
     * @brief lhs AND rhs
     */
    Expression and_(const Expression& lhs, const Expression& rhs) const;
    
    /**
     * @brief lhs OR rhs
     */
    Expression or_(const Expression& lhs, const Expression& rhs) const;

    /**
     * @brief NOT exp
     */
    Expression not_(const Expression& exp) const;
    //@}
    
    /**
     * @name arithmetic operators
     * @{
     */
    /**
     * @brief lhs + rhs
     */
    Expression add(const Expression& lhs, const Expression& rhs) const;

    Expression column(const std::string& table,
                            const std::string& column) const;

    Expression label(const Expression& x, const std::string& label) const;

    Expression alias(const Expression& x, const std::string& alias) const;

    Expression table(const std::string& table) const;

    Expression min(const Expression& x) const;
    Expression max(const Expression& x) const;
    Expression sum(const Expression& x) const;
    Expression count(const Expression& x) const;


    Expression binary_op(const std::string& op,
                               const Expression& lhs,
                               const Expression& rhs) const;

    Expression unary_op(const std::string& op,
                              const Expression& arg) const;
};


}
}

#endif // BRFC_SQL_FACTORY_HPP
