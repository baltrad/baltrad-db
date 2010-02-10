#ifndef BRFC_TEST_COMMON_HPP
#define BRFC_TEST_COMMON_HPP

#include <iosfwd>

class QVariant;
class QDate;
class QTime;

namespace brfc {
    class Variant;
}

std::ostream& operator<<(std::ostream& out, const QVariant& value);
std::ostream& operator<<(std::ostream& out, const QDate& value);
std::ostream& operator<<(std::ostream& out, const QTime& value);
std::ostream& operator<<(std::ostream& out, const brfc::Variant& value);

#endif // BRFC_TEST_COMMON_HPP
