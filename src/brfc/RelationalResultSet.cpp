#include <brfc/RelationalResultSet.hpp>

#include <QtCore/QVariant>

#include <QtSql/QSqlQuery>

namespace brfc {

RelationalResultSet::RelationalResultSet(const QSqlQuery& query)
        : query_(new QSqlQuery(query)) {
    query_->seek(-1);
}

RelationalResultSet::RelationalResultSet(const RelationalResultSet& other)
        : query_(new QSqlQuery(*other.query_)) {
}

RelationalResultSet&
RelationalResultSet::operator=(const RelationalResultSet& rhs) {
    if (this != &rhs) {
        query_.reset(new QSqlQuery(*rhs.query_));
    }
    return *this;
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

QVariant
RelationalResultSet::do_value_at(unsigned int pos) const {
    return query_->value(pos);
}

} // namespace brfc
