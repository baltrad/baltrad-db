#include <gtest/gtest.h>

#include <brfc/Variant.hpp>
#include <brfc/exceptions.hpp>

#include "common.hpp"

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
    QString val = QString::fromUtf8("öäõxyz");
    Variant v(val);
    EXPECT_EQ(v.type(), Variant::STRING);
    EXPECT_EQ(v.string(), val);
    EXPECT_FALSE(v.is_null());
    EXPECT_THROW(v.longlong(), value_error);
}

TEST(Variant_test, from_invalid_qvariant) {
    QVariant qv;
    Variant v(qv);
    EXPECT_EQ(v.type(), Variant::NONE);
    EXPECT_TRUE(v.is_null());
}

TEST(Variant_test, from_int_qvariant) {
    Variant v(QVariant(1));
    EXPECT_EQ(v.type(), Variant::LONGLONG);
    EXPECT_EQ(v.longlong(), 1);
}

TEST(Variant_test, null_to_qvariant) {
    Variant v;
    QVariant qv = v.to_qvariant();
    EXPECT_EQ(qv.type(), QVariant::Invalid);
    EXPECT_TRUE(qv.isNull());
}

TEST(Variant_test, longlong_to_qvariant) {
    Variant v(1);
    QVariant qv = v.to_qvariant();
    EXPECT_EQ(qv.type(), QVariant::LongLong);
    EXPECT_EQ(qv.toLongLong(), 1);
}

TEST(Variant_test, from_string_literal) {
    Variant v("asd");
    EXPECT_EQ(v.type(), Variant::STRING);
}

TEST(Variant_test, equality_comparison) {
    Variant v1(1);
    Variant v2;
    Variant v3(1);
    Variant v4("asdasd");
    EXPECT_EQ(v1, v1);
    EXPECT_NE(v1, v2);
    EXPECT_EQ(v1, v3);
    EXPECT_NE(v1, v4);
    EXPECT_EQ(v2, v2);
    EXPECT_NE(v2, v3);
    EXPECT_NE(v2, v4);
    EXPECT_EQ(v3, v3);
    EXPECT_NE(v3, v4);
    EXPECT_EQ(v4, v4);
}

TEST(Variant_test, null_to_string) {
    EXPECT_EQ(Variant().to_string(), "");
}

TEST(Variant_test, int_to_string) {
    EXPECT_EQ(Variant(10).to_string(), "10");
}

TEST(Variant_test, double_to_string) {
    EXPECT_EQ(Variant("1.1").to_string(), "1.1");
}

TEST(Variant_test, bool_to_string) {
    EXPECT_EQ(Variant(true).to_string(), "True");
    EXPECT_EQ(Variant(false).to_string(), "False");
}

TEST(Variant_test, date_to_string) {
    EXPECT_EQ(Variant(QDate(2000, 11, 12)).to_string(), "20001112");
}

TEST(Variant_test, time_to_string) {
    EXPECT_EQ(Variant(QTime(12, 34, 56)).to_string(), "123456");
}
