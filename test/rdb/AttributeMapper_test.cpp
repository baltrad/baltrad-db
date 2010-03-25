#include <gtest/gtest.h>

#include <brfc/rdb/AttributeMapper.hpp>

#include "../common.hpp"

namespace brfc {
namespace rdb {

class rdb_AttributeMapper_test : public testing::Test {
  public:
    static void SetUpTestCase() {
        mapper.add(Mapping(0, "attr1", "table1", "column1"));
        mapper.add(Mapping(1, "attr2", "table1", "column2"));
        mapper.add(Mapping(2, "attr3", "table2", "column1"));
        mapper.add(Mapping(3, "attr4", "attribute_values_str", "value"));
    }

    static AttributeMapper mapper;
};

AttributeMapper rdb_AttributeMapper_test::mapper;

TEST_F(rdb_AttributeMapper_test, specializations_on) {
    MappingVector v = mapper.specializations_on("table1");
    ASSERT_EQ(v.size(), 2);
    EXPECT_EQ(v.at(0).attribute, "attr1");
    EXPECT_EQ(v.at(1).attribute, "attr2");
}

TEST_F(rdb_AttributeMapper_test, is_specialized) {
    EXPECT_TRUE(mapper.is_specialized("attr1"));
    EXPECT_FALSE(mapper.is_specialized("attr4"));
}

} // namespace rdb
} // namespace brfc

