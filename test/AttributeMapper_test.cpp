#include <boost/test/unit_test.hpp>

#include <brfc/AttributeMapper.hpp>

using namespace brfc;

namespace {
struct Fixture {
    Fixture()
            : mapper() {
    }

    AttributeMapper mapper;
};

} // namespace anonymouse

BOOST_FIXTURE_TEST_SUITE(AttributeMapper_test, Fixture)

BOOST_AUTO_TEST_CASE(specializations_on) {
    mapper.add_spec(0, "attr1", "string", "table1", "column1");
    mapper.add_spec(1, "attr2", "string", "table1", "column2");
    mapper.add_spec(2, "attr3", "string", "table2", "column1");

    MappingVector v = mapper.specializations_on("table1");
    BOOST_REQUIRE_EQUAL(v.size(), 2);
    BOOST_CHECK_EQUAL(v.at(0).attribute, "attr1");
    BOOST_CHECK_EQUAL(v.at(1).attribute, "attr2");
}

BOOST_AUTO_TEST_SUITE_END()
