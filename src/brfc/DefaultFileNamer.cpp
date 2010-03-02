#include <brfc/DefaultFileNamer.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/File.hpp>
#include <brfc/Source.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

DefaultFileNamer::DefaultFileNamer() {
}

QString
DefaultFileNamer::do_name(const File& f) const {
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
