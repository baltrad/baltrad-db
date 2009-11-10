#include <brfc/Source.hpp>
#include <boost/test/unit_test.hpp>

#include <brfc/assert.hpp>

#include <stdexcept>

#include "common.hpp"

using namespace brfc;

BOOST_AUTO_TEST_SUITE(Source_test)

BOOST_AUTO_TEST_CASE(valid_source) {
    Source s("WMO:02606,RAD:SE50");
    BOOST_REQUIRE(s.wmo_code().isValid());
    BOOST_REQUIRE(s.radar_site().isValid());
    BOOST_CHECK_EQUAL(s.wmo_code().toInt(), 2606);
    BOOST_CHECK_EQUAL(s.radar_site().toString(), "SE50");
}

BOOST_AUTO_TEST_CASE(invalid_source1) {
    BOOST_CHECK_THROW(Source(""), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid_source2) {
    BOOST_CHECK_THROW(Source("WMO:,"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(invalid_source3) {
    BOOST_CHECK_THROW(Source("WMO:"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
