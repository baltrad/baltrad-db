#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/SourceRadar.hpp>
#include <brfc/oh5/SourceCentre.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

TEST(oh5_Source_test, valid_source) {
    QString value = QString::fromUtf8("WMO:02606,RAD:SE50");
    shared_ptr<Source> s = Source::from_source_attribute(value);

    shared_ptr<SourceRadar> rad = dynamic_pointer_cast<SourceRadar>(s);

    EXPECT_TRUE(rad);
    
    EXPECT_EQ(rad->wmo_code(), 2606);

    EXPECT_EQ(rad->radar_site(), "SE50");

    EXPECT_EQ(rad->place(), "");
}

TEST(oh5_Source_test, from_empty_source) {
    QString value = QString::fromUtf8("");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}

TEST(oh5_Source_test, from_source_missing_value) {
    QString value = QString::fromUtf8("WMO:");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}

TEST(oh5_Source_test, from_source_with_invalid_key) {
    QString value = QString::fromUtf8("asd:qwe");
    EXPECT_THROW(Source::from_source_attribute(value), value_error);
}

TEST(oh5_Source_test, radar_to_string) {
    SourceRadar radar;
    radar.wmo_code(2606);
    EXPECT_EQ(radar.to_string(), "WMO:2606");
    radar.radar_site("SE50");
    EXPECT_EQ(radar.to_string(), "WMO:2606,RAD:SE50");
    radar.place("Ängelholm");
    EXPECT_EQ(radar.to_string(), "WMO:2606,RAD:SE50,PLC:Ängelholm");
}

TEST(oh5_Source_test, centre_to_string) {
    SourceCentre centre;
    centre.originating_centre(247);
    EXPECT_EQ(centre.to_string(), "ORG:247");
    centre.country_code(613);
    EXPECT_EQ(centre.to_string(), "ORG:247,CTY:613");
}

} // namespace oh5
} // namespace brfc
