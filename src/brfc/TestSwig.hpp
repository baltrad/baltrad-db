#ifndef BRFC_TEST_SWIG_HPP
#define BRFC_TEST_SWIG_HPP

#include <string>
#include <QtCore/QString>

namespace brfc {

class TestSwig {
  public:
    std::string
    qstring_to_string(QString str);

    std::string
    qstringconstref_to_string(const QString& str);

    QString
    string_to_qstring(const std::string& str);

    const QString&
    string_to_qstringconstref(const std::string& str);

  private:
    QString for_constref_;
};

} // namespace brfc

#endif // BRFC_TEST_SWIG_HPP
