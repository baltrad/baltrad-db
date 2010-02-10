#include "common.hpp"

#include <QtCore/QVariant>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <brfc/Variant.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& out, const QVariant& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const QDate& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const QTime& value) {
    out << value.toString().toStdString();
    return out;
}

std::ostream& operator<<(std::ostream& out, const brfc::Variant& value) {
    out << value.to_qvariant().toString().toStdString();
    return out;
}
