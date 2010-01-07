#include <gtest/gtest.h>

#include <brfc/Source.hpp>

#include <stdexcept>

#include "common.hpp"

using namespace brfc;

TEST(Source_test, valid_source) {
    Source s("WMO:02606,RAD:SE50");
    ASSERT_TRUE(s.wmo_code().isValid());
    ASSERT_TRUE(s.radar_site().isValid());
    EXPECT_EQ(s.wmo_code().toInt(), 2606);
    EXPECT_EQ(s.radar_site().toString(), "SE50");
}

TEST(Source_test, invalid_source1) {
    EXPECT_THROW(Source(""), std::runtime_error);
}

TEST(Source_test, invalid_source2) {
    EXPECT_THROW(Source("WMO:,"), std::runtime_error);
}

TEST(Source_test, invalid_source3) {
    EXPECT_THROW(Source("WMO:"), std::runtime_error);
}
