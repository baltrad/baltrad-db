#include <brfc/Query.hpp>

#include <brfc/Database.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Expression.hpp>

namespace brfc {

Query::Query(Database* db, const AttributeMapper* mapper)
        : db_(db)
        , mapper_(mapper)
        , distinct_()
        , fetch_()
        , filter_() {
}

Query::Query(const Query& other)
        : db_(other.db_)
        , mapper_(other.mapper_)
        , distinct_(other.distinct_)
        , fetch_(other.fetch_)
        , filter_(other.filter_) {

}

Query::~Query() {
    
}

Query&
Query::distinct(bool distinct) {
    distinct_ = true;
    return *this;
}

Query&
Query::fetch(expr::AttributePtr attribute) {
    fetch_.push_back(attribute);
    return *this;
}

Query&
Query::filter(expr::ExpressionPtr expr) {
    filter_ = filter_ ? filter_->and_(expr) : expr;
    return *this;
}

shared_ptr<ResultSet>
Query::execute() {
    return db_->query(*this);
}

} // namespace brfc
