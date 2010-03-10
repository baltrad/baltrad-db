#ifndef BRFC_TEMPORARY_H5_FILE_HPP
#define BRFC_TEMPORARY_H5_FILE_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/oh5/hlhdf.hpp>

class QString;

namespace brfc {

class Variant;

namespace oh5 {

class File;

}

namespace test {

class TempH5File {
  public:
    /**
     * @brief constructor
     *
     * creates a temporary file
     */
    TempH5File();

    /**
     * @brief destructor
     *
     * unlinks the file
     */
    ~TempH5File();
    
    void write(const oh5::File& f);
    
    QString filename() const;

  private:
    shared_ptr<char> filename_;
};

} // namepsace test
} // namespace brfc

#endif // BRFC_TEMPORARY_H5_FILE_HPP
