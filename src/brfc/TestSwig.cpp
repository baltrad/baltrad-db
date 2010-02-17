#include <brfc/TestSwig.hpp>

namespace brfc {

std::string
TestSwig::qstring_to_string(QString str) {
    return std::string(str.toUtf8().constData());
}

std::string
TestSwig::qstringconstref_to_string(const QString& str) {
    return std::string(str.toUtf8().constData());
}

QString
TestSwig::string_to_qstring(const std::string& str) {
    return QString::fromUtf8(str.c_str());
}

const QString&
TestSwig::string_to_qstringconstref(const std::string& str) {
    for_constref_ = QString::fromUtf8(str.c_str());
    return for_constref_;
}

} // namespace brfc
