#include <brfc/FileNamer.hpp>

namespace brfc {

QString
FileNamer::inject_version(const QString& filename,
                          unsigned int version) {
    QString vstring = QString::number(version).rightJustified(6, '0');
    QString newname(filename);
    if (filename.contains("$VERSION$")) {
        newname.replace("$VERSION$", vstring);
    } else if (filename.contains(".")) {
        int last_dot_pos = newname.lastIndexOf('.');
        newname.insert(last_dot_pos, "_" + vstring);
    } else {
        newname.append("_" + vstring);
    }
    return newname;
}

} // namespace brfc
