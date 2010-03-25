#include <brfc/rdb/RelationalResultSet.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <QtCore/QVariant>
#include <QtSql/QSqlQuery>

namespace brfc {
namespace rdb {

RelationalResultSet::RelationalResultSet(const QSqlQuery& query)
        : query_(new QSqlQuery(query)) {
    query_->seek(-1);
}

RelationalResultSet::~RelationalResultSet() {

}

bool
RelationalResultSet::do_next() {
    return query_->next();
}

bool
RelationalResultSet::do_seek(int idx) {
    return query_->seek(idx);
}

int
RelationalResultSet::do_size() {
    return query_->size();
}

Variant
RelationalResultSet::do_value_at(unsigned int pos) const {
    const QVariant& var = query_->value(pos);
    if (not var.isValid()) {
        throw lookup_error("invalid position");
    }
    return Variant(var);
}

} // namespace rdb
} // namespace brfc
