#include <gtest/gtest.h>

#include <brfc/FileNamer.hpp>

#include "common.hpp"

namespace brfc {

TEST(FileNamer_test, with_marked_version) {
    QString original("filename_$VERSION$.ext");
    unsigned int version = 1;
    QString expected("filename_000001.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, with_dots) {
    QString original("file.name.ext");
    unsigned int version = 1;
    QString expected("file.name_000001.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, without_dots) {
    QString original("filename");
    unsigned int version = 1;
    QString expected("filename_000001");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

TEST(FileNamer_test, large_number) {
    QString original("filename_$VERSION$.ext");
    unsigned int version = 1234567;
    QString expected("filename_1234567.ext");
    EXPECT_EQ(expected, FileNamer::inject_version(original, version));
}

} // namespace brfc
