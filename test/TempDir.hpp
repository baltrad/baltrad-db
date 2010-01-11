#ifndef BRFC_TEST_TEMP_DIR_HPP
#define BRFC_TEST_TEMP_DIR_HPP

#include <brfc/smart_ptr.hpp>

#include <string>
#include <vector>

namespace brfc {

class TempDir {
  public:
    typedef std::vector<std::string> FileVector;

    TempDir();
    ~TempDir();
    std::string path() const;

  private:
    void rmdir_recurse(const char* path);

    shared_ptr<char> path_;
};

} // namespace brfc

#endif // BRFC_TEST_TEMP_DIR_HPP
