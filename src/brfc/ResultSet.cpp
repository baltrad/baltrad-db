#include <brfc/ResultSet.hpp>

#include <QtCore/QVariant>
#include <QtCore/QDate>
#include <QtCore/QTime>

#include <brfc/exceptions.hpp>

namespace brfc {

std::string
ResultSet::string(unsigned int pos) const {
    return value_at(pos).toString().toStdString();
}

long long
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

QVariant
ResultSet::value_at(unsigned int pos) const {
    const QVariant& v = do_value_at(pos);
    if (not v.isValid()) {
        throw lookup_error("invalid position");
    }
    return v;
}

} // namespace brfc
