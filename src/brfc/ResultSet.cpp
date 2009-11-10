#include <brfc/ResultSet.hpp>

#include <brfc/exceptions.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include <QtSql/QSqlQuery>

namespace brfc {

ResultSet::ResultSet(const QSqlQuery& query)
        : query_(new QSqlQuery(query)) {
    query_->seek(-1);
}

ResultSet::ResultSet(const ResultSet& other)
        : query_(new QSqlQuery(*other.query_)) {
}

ResultSet&
ResultSet::operator=(const ResultSet& rhs) {
    if (this != &rhs) {
        query_.reset(new QSqlQuery(*rhs.query_));
    }
    return *this;
}

ResultSet::~ResultSet() {

}

bool
ResultSet::next() {
    return query_->next();
}

bool
ResultSet::seek(int idx) {
    return query_->seek(idx);
}

int
ResultSet::size() {
    return query_->size();
}

QVariant
ResultSet::value_at(unsigned int pos) const {
    const QVariant& v = query_->value(pos);
    if (not v.isValid()) {
        throw lookup_error("invalid position");
    }
    return v;
}

std::string
ResultSet::string(unsigned int pos) const {
    return value_at(pos).toString().toStdString();
}

int64_t
ResultSet::integer(unsigned int pos) const {
    return value_at(pos).toLongLong();
}

double
ResultSet::real(unsigned int pos) const {
    return value_at(pos).toDouble();
}

bool
ResultSet::boolean(unsigned int pos) const {
    return value_at(pos).toBool();
}

QDate
ResultSet::date(unsigned int pos) const {
    return value_at(pos).toDate();
}

QTime
ResultSet::time(unsigned int pos) const {
    return value_at(pos).toTime();
}

} // namespace brfc
