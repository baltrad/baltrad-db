#ifndef BRFC_QUERY_HPP
#define BRFC_QUERY_HPP

#include <brfc/expr/fwd.hpp>

#include <vector>

namespace brfc {

class Database;
class ResultSet;

/**
 * @brief query against the database
 *
 * @ingroup exposed_in_binds
 */
class Query {
  public:
    typedef std::vector<expr::AttributePtr> AttributeVector;

    /**
     * @brief constructor
     * @param db Database instance this Query executes on
     */
    Query(Database* db);

    /**
     * @brief copy constructor
     */
    Query(const Query& other);
    
    /**
     * @brief destructor
     */
    ~Query();

    /**
     * @brief fetch unique results
     */
    Query& distinct(bool distinct);

    bool distinct() const { return distinct_; }

    /**
     * @brief mark an attribute for fetching
     * @param attribute Attribute to fetch to ResultSet
     * @return this Query (for chaining)
     *
     * attributes are returned in ResultSet in the same order as they
     * are marked.
     */
    Query& fetch(expr::AttributePtr attribute);

    const AttributeVector& fetch() const {
        return fetch_;
    }

    /**
     * @brief add a filtering expression
     * @param expr filter expression
     * @return this Query (for chaining)
     *
     * successive filtering expressions are added together using AND
     */
    Query& filter(expr::ExpressionPtr expr);
    
    expr::ExpressionPtr filter() const {
        return filter_;
    }

    /**
     * @brief execute this query
     * @return ResultSet containing executed query results
     *
     * each row in the results contains a value for each attribute marked
     * for fetching, in the order they were marked.
     */
    shared_ptr<ResultSet> execute();

  private:
    Database* db_;
    bool distinct_;
    AttributeVector fetch_;
    expr::ExpressionPtr filter_;
};

} // namespace brfc

#endif // BRFC_QUERY_HPP
