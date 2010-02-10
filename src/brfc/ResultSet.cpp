#include <brfc/ResultSet.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

std::string
ResultSet::string(unsigned int pos) const {
    return value_at(pos).string();
}

long long
ResultSet::integer(unsigned int pos) const {
    return value_at(pos).longlong();
}

double
ResultSet::real(unsigned int pos) const {
    return value_at(pos).double_();
}

bool
ResultSet::boolean(unsigned int pos) const {
    return value_at(pos).bool_();
}

QDate
ResultSet::date(unsigned int pos) const {
    return value_at(pos).date();
}

QTime
ResultSet::time(unsigned int pos) const {
    return value_at(pos).time();
}

Variant
ResultSet::value(unsigned int pos) const {
    return value_at(pos);
}

Variant
ResultSet::value_at(unsigned int pos) const {
    return do_value_at(pos);
}

} // namespace brfc
