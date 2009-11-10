#include "common.hpp"

#include <QtCore/QVariant>
#include <iostream>

std::ostream& operator<<(std::ostream& out, const QVariant& value) {
    out << value.toString().toStdString();
    return out;
}

