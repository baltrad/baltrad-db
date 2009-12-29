#include <brfc/Query.hpp>

#include <brfc/assert.hpp>
#include <brfc/Database.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/AttrReplace.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/FromClause.hpp>
#include <brfc/expr/Parentheses.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Table.hpp>

namespace brfc {

Query::Query(Database* db, const AttributeMapper* mapper)
        : db_(db)
        , mapper_(mapper)
        , select_(expr::Select::create()) {
}

Query::Query(const Query& other)
        : db_(other.db_)
        , select_(other.select_) {
}

Query::~Query() {
    
}

Query&
Query::distinct(bool distinct) {
    select_->distinct(distinct);
    return *this;
}

Query&
Query::fetch(expr::AttributePtr attribute) {
    select_->what(attribute);
    return *this;
}

Query&
Query::filter(expr::ExpressionPtr expr) {
    // replace attribute
    select_->append_where(expr);
    return *this;
}

ResultSet
Query::execute() {
    expr::TablePtr data_objects_t = expr::Table::create("data_objects");
    expr::TablePtr files_t = expr::Table::create("files");
    select_->from()->add(files_t);

    expr::AttrReplace r(mapper_);
    r.replace(select_);
    expr::Compiler compiler;
    compiler.compile(*select_);

    return db_->query(QString::fromStdString(compiler.compiled()), compiler.binds());
}

} // namespace brfc
