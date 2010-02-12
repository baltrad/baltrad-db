#include <gtest/gtest.h>

#include "common.hpp"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new brfc::TestRDBEnv);
    return RUN_ALL_TESTS();
}
