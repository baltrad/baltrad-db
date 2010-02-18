#include <brfc/DefaultFileNamer.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/File.hpp>

#include <QtCore/QDir>

namespace brfc {

DefaultFileNamer::DefaultFileNamer(const QString& path)
        : path_(path) {
    QDir dir_(path);
    if (not dir_.isAbsolute())
        throw fs_error("storage must be an absolute path");
    if (not dir_.exists())
        throw fs_error("storage does not exist");
}

QString
DefaultFileNamer::do_name(const File& f) const {
    return path_ + "/" + f.unique_identifier() + ".h5";
}

} // namespace brfc
