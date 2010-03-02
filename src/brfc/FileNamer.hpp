#ifndef BRFC_FILE_NAMER_HPP
#define BRFC_FILE_NAMER_HPP

#include <QtCore/QString>

namespace brfc {

class File;

/**
 * @brief ABC for generating filenames for File instances
 */
class FileNamer {
  public:
    /**
     * @brief name a File
     * @param file File to be named
     * @return the generated filename
     *
     * @sa do_name
     */
    QString name(const File& file) const {
        return do_name(file);
    }

    /**
     * @brief destructor
     */
    virtual ~FileNamer() { }

  protected:
    virtual QString do_name(const File& file) const = 0;
};

} // namespace brfc

#endif // BRFC_FILE_NAMER_HPP
