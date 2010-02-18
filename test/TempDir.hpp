#ifndef BRFC_TEST_TEMP_DIR_HPP
#define BRFC_TEST_TEMP_DIR_HPP

#include <brfc/smart_ptr.hpp>

class QString;

namespace brfc {

class TempDir {
  public:
    TempDir();
    ~TempDir();
    QString path() const;

  private:
    void rmdir_recurse(const char* path);

    shared_ptr<char> path_;
};

} // namespace brfc

#endif // BRFC_TEST_TEMP_DIR_HPP
