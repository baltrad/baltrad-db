/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BRFC_EXPR_ATTR_REPLACE_HPP
#define BRFC_EXPR_ATTR_REPLACE_HPP

#include <brfc/visit.hpp>
#include <brfc/expr/fwd.hpp>

#include <vector>

namespace brfc {

namespace rdb {

class AttributeMapper;

}

namespace expr {


/**
 * @brief replace expr::Attribute with expr::Column
 *
 * The strategy is to first replace all the attributes with columns,
 * collecting the unique tables/aliases. Since the Select has no from
 * clause, we determine a central table and join the remaining tables to it
 */
class AttrReplace {
  public:
    typedef mpl::vector<
                        Attribute,
                        BinaryOperator,
                        Label,
                        Literal,
                        Parentheses
                        > accepted_types;

    /**
     * @brief replace attributes in a Select statement
     */
    static void replace(SelectPtr select, const rdb::AttributeMapper* mapper);

    /**
     * @brief visit an Attribute element
     *
     * replace this attribute with a column
     * if it's not a specialized column,
     * add a where clause to attribute names
     */
    void operator()(Attribute& attr);

    void operator()(BinaryOperator& op);

    void operator()(Label& label);

    void operator()(Literal& literal);

    void operator()(Parentheses& parentheses);

 protected: 
    /**
     * @brief constructor
     * @param mapper AttributeMapper instance to fetch mappings from
     */
    AttrReplace(SelectPtr select, const rdb::AttributeMapper* mapper);

    void replace_attributes();

    void build_from_clause();

    ElementPtr pop();

    void push(ElementPtr p);

    void join_groups();

  private:
    const rdb::AttributeMapper* mapper_;
    std::vector<ElementPtr> stack_;
    SelectPtr select_;
    JoinPtr from_;
};

} // namespace expr
} // namespace brfc

#endif // BRFC_EXPR_ATTR_REPLACE_HPP
