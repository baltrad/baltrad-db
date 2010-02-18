#include "TempDir.hpp"

#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <iostream>
#include <QtCore/QString>

namespace brfc {

TempDir::TempDir()
        : path_(strdup("/tmp/brfctest_XXXXXX"), &free) {
    if (mkdtemp(path_.get()) == 0) {
        throw std::runtime_error("could not create temporary directory");
    }
}

TempDir::~TempDir() {
    rmdir_recurse(path_.get());
}

QString
TempDir::path() const {
    return QString::fromUtf8(path_.get());
}

void
TempDir::rmdir_recurse(const char* path) {
    DIR* dir = opendir(path);
    if (dir == 0)
        return;

    struct dirent* ent = 0;
    for (ent = readdir(dir); ent != 0; ent = readdir(dir)) {
        if (strcmp(ent->d_name, ".") == 0 or strcmp(ent->d_name, "..") == 0)
            continue;
        std::string full_path = std::string(path) + "/" + ent->d_name;
        if (ent->d_type == DT_DIR) {
            rmdir_recurse(full_path.c_str());
        } else {
            if (unlink(full_path.c_str()) == -1)
                std::cerr << "could not unlink " << full_path
                          << ": " << strerror(errno) << std::endl;
        }
    }
    closedir(dir);
    if (rmdir(path) == -1)
        std::cerr << "could not unlink " << path
                  << ": " << strerror(errno) << std::endl;
}

} // namespace brfc
