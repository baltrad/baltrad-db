#include <gtest/gtest.h>

#include <brfc/Source.hpp>
#include <brfc/exceptions.hpp>

#include <QtCore/QString>

using namespace brfc;

TEST(Source_test, valid_source) {
    QString value = QString::fromUtf8("WMO:02606,RAD:SE50");
    const Source& s = Source::from_source_attribute(value);
    
    EXPECT_EQ(s.wmo_code(), 2606);

    EXPECT_EQ(s.radar_site(), "SE50");

    EXPECT_EQ(s.originating_centre(), 0);
    EXPECT_EQ(s.place(), "");
    EXPECT_EQ(s.country_code(), 0);
}

TEST(Source_test, from_empty_source) {
    QString value = QString::fromUtf8("");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}

TEST(Source_test, from_source_missing_value) {
    QString value = QString::fromUtf8("WMO:");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}

TEST(Source_test, from_source_with_invalid_key) {
    QString value = QString::fromUtf8("asd:qwe");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}
