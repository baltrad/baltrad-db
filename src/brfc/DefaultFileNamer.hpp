#ifndef BRFC_DEFAULT_FILE_NAMER_HPP
#define BRFC_DEFAULT_FILE_NAMER_HPP

#include <brfc/FileNamer.hpp>

namespace brfc {

class DefaultFileNamer : public FileNamer {
  public:
    DefaultFileNamer(const QString& path);

  protected:
    virtual QString do_name(const File& f) const;
  
  private:
    QString path_;
};

} // namespace brfc

#endif // BRFC_DEFAULT_FILE_NAMER_HPP
