#include <gtest/gtest.h>

#include <brfc/Source.hpp>
#include <brfc/exceptions.hpp>

#include "common.hpp"

using namespace brfc;

TEST(Source_test, valid_source) {
    Source s("WMO:02606,RAD:SE50");

    EXPECT_TRUE(s.wmo_code().isValid());
    EXPECT_EQ(s.wmo_code().toInt(), 2606);

    EXPECT_TRUE(s.radar_site().isValid());
    EXPECT_EQ(s.radar_site().toString(), "SE50");

    EXPECT_FALSE(s.originating_centre().isValid());
    EXPECT_FALSE(s.place().isValid());
    EXPECT_FALSE(s.country_code().isValid());
}

TEST(Source_test, from_empty_source) {
    EXPECT_THROW(Source(""), value_error);
}

TEST(Source_test, from_source_missing_value) {
    EXPECT_THROW(Source("WMO:"), value_error);
}

TEST(Source_test, from_source_with_invalid_key) {
    EXPECT_THROW(Source("asd:qwe"), value_error);
}
