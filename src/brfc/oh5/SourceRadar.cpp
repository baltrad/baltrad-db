#include <brfc/oh5/SourceRadar.hpp>

#include <QtCore/QStringList>

namespace brfc {
namespace oh5 {

QString
SourceRadar::to_string() const {
    QStringList elems;
    if (wmo_code_ != 0)
        elems.append("WMO:" + QString::number(wmo_code_));
    if (radar_site_ != "")
        elems.append("RAD:" + radar_site_);
    if (place_ != "")
        elems.append("PLC:" + place_);
    return elems.join(",");
}

} // namespace oh5
} // namespace brfc
