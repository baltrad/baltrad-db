#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/DefaultFileNamer.hpp>

namespace brfc {

TEST(DefaultFileNamer_test, test_nx_storage_path_throws) {
    EXPECT_THROW(DefaultFileNamer("/path/to/nxdir"), fs_error);
}

} // namespace brfc
