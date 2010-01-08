#include <gtest/gtest.h>

#include <brfc/AttributeMapper.hpp>

using namespace brfc;

namespace {
struct AttributeMapper_test : public testing::Test {
    AttributeMapper_test()
            : mapper() {
    }

    AttributeMapper mapper;
};

} // namespace anonymouse

TEST_F(AttributeMapper_test, specializations_on) {
    mapper.add(Mapping(0, "attr1", "table1", "column1"));
    mapper.add(Mapping(1, "attr2", "table1", "column2"));
    mapper.add(Mapping(2, "attr3", "table2", "column1"));

    MappingVector v = mapper.specializations_on("table1");
    ASSERT_EQ(v.size(), 2);
    EXPECT_EQ(v.at(0).attribute, "attr1");
    EXPECT_EQ(v.at(1).attribute, "attr2");
}
