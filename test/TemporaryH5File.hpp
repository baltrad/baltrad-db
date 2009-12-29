#ifndef BRFC_TEMPORARY_H5_FILE_HPP
#define BRFC_TEMPORARY_H5_FILE_HPP

#include <brfc/hlhdf.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/Converter.hpp>

class QVariant;

namespace brfc {

class TemporaryH5File {
  public:
    TemporaryH5File();
    ~TemporaryH5File();
    void add_group(const char* path);
    void add_attribute(const char* path, const QVariant& value);
    void write();
    const char* filename() const;

  private:
    HL_Data convert(const QVariant& value);
    
    shared_ptr<HL_NodeList> nodes_;
    shared_ptr<char> filename_;
    int fd_;
};

} // namespace brfc

#endif // BRFC_TEMPORARY_H5_FILE_HPP
