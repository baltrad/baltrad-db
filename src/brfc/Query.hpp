#ifndef BRFC_QUERY_HPP
#define BRFC_QUERY_HPP

#include <boost/scoped_ptr.hpp>

#include <brfc/expr/fwd.hpp>

namespace brfc {

class AttributeMapper;
class Database;
class ResultSet;

/**
 * @brief query against the database
 *
 * @ingroup exposed_in_binds
 */
class Query {
  public:
    /**
     * @brief constructor
     * @param db Database instance this Query executes on
     * @param mapper AttributeMapper instance used to solve
     *        Attributes to Columns
     */
    Query(Database* db, const AttributeMapper* mapper);

    /**
     * @brief copy constructor
     */
    Query(const Query& other);
    
    /**
     * @brief destructor
     */
    ~Query();

    /**
     * @brief fetch unique rows
     */
    Query& distinct(bool distinct=true);

    /**
     * @brief mark an attribute for fetching
     * @param attribute Attribute to fetch to ResultSet
     * @return this Query (for chaining)
     *
     * attributes are returned in ResultSet in the same order as they
     * are marked.
     */
    Query& fetch(expr::AttributePtr attribute);

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this Query (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    Query& filter(expr::ExpressionPtr expr);

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    ResultSet execute();

  private:
    Database* db_;
    const AttributeMapper* mapper_;
    expr::SelectPtr select_;
};

} // namespace brfc

#endif // BRFC_QUERY_HPP
