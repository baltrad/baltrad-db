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
    const DataObject& r = f.root();

    QString name = QString("Z_");
    name.append(r.attribute("what/object").value().string());
    name.append("_C_");
    name.append(f.source()->wmo_cccc());
    name.append("_");
    name.append(r.attribute("what/date").value().date().toString("yyyyMMdd"));
    name.append(r.attribute("what/time").value().time().toString("hhmmss"));
    name.append("_");
    name.append(f.source()->node_id());
    name.append(".h5");

    return name;
}

} // namespace brfc
