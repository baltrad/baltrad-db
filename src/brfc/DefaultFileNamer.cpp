#include <brfc/DefaultFileNamer.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {

DefaultFileNamer::DefaultFileNamer() {
}

QString
DefaultFileNamer::do_name(const oh5::File& f) const {
    QString name = QString("Z_");
    name.append(f.what_object());
    name.append("_C_");
    name.append(f.source()->wmo_cccc());
    name.append("_");
    name.append(f.what_date().toString("yyyyMMdd"));
    name.append(f.what_time().toString("hhmmss"));
    name.append("_");
    name.append(f.source()->node_id());
    name.append(".h5");

    return name;
}

} // namespace brfc
