#ifndef BRFC_FILE_NAMER_HPP
#define BRFC_FILE_NAMER_HPP

#include <QtCore/QString>

namespace brfc {

class File;

class FileNamer {
  public:
    QString name(const File& f) const {
        return do_name(f);
    }

    virtual ~FileNamer() { }

  protected:
    virtual QString do_name(const File& f) const = 0;
};

} // namespace brfc

#endif // BRFC_FILE_NAMER_HPP
