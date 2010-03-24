#include <brfc/oh5/SourceCentre.hpp>

#include <QtCore/QStringList>

namespace brfc {
namespace oh5 {

QString
SourceCentre::to_string() const {
    QStringList elems;
    if (originating_centre_ != 0)
        elems.append("ORG:" + QString::number(originating_centre_));
    if (country_code_ != 0)
        elems.append("CTY:" + QString::number(country_code_));
    return elems.join(",");
}

} // namespace oh5
} // namespace brfc
