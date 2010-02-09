#include <gtest/gtest.h>

#include <brfc/Variant.hpp>
#include <brfc/exceptions.hpp>

using namespace brfc;

TEST(Variant_test, null) {
    Variant v;
    EXPECT_TRUE(v.is_null());
    EXPECT_EQ(v.type(), Variant::NONE);
    EXPECT_FALSE(v.is_string());
    EXPECT_THROW(v.longlong(), value_error);
}

TEST(Variant_test, longlong) {
    Variant v(10); // accepts integers
    EXPECT_FALSE(v.is_null());
    EXPECT_EQ(v.longlong(), 10);
    EXPECT_EQ(v.type(), Variant::LONGLONG);
    EXPECT_THROW(v.string(), value_error);
}

TEST(Variant_test, copy_ctor) {
    Variant v1(10);
    Variant v2 = v1;
    EXPECT_FALSE(v2.is_null());
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.longlong(), v1.longlong());
}

TEST(Variant_test, copy_assign) {
    Variant v1(10);
    Variant v2;
    v2 = v1;
    EXPECT_FALSE(v2.is_null());
    EXPECT_EQ(v2.type(), v1.type());
    EXPECT_EQ(v2.longlong(), v1.longlong());
}

TEST(Variant_test, string) {
    Variant v("öäüxyz");
    EXPECT_EQ(v.type(), Variant::STRING);
    EXPECT_EQ(v.string(), "öäüxyz");
    EXPECT_FALSE(v.is_null());
    EXPECT_THROW(v.longlong(), value_error);
}

TEST(Variant_test, qstring) {
    Variant v("öäüxyz");
    EXPECT_EQ(v.qstring().length(), 6);
}
