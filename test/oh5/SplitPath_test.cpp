#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/oh5/SplitPath.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

class oh5_SplitPath_test : public ::testing::Test {
  public:
    oh5_SplitPath_test() {
    }

    virtual void SetUp() {
    }
};

// TEST FOR DEPRECATED FEATURES
TEST_F(oh5_SplitPath_test, split_short_path_without_group) {
    SplitPath p("/Conventions");
    EXPECT_EQ(p.group_path(), "/");
    EXPECT_EQ(p.attribute_name(), "Conventions");
    EXPECT_EQ(p.full_attribute_name(), "Conventions");
}

TEST_F(oh5_SplitPath_test, split_long_path_without_group) {
    SplitPath p("/dataset1/data1/object");
    EXPECT_EQ(p.group_path(), "/dataset1/data1");
    EXPECT_EQ(p.attribute_name(), "object");
    EXPECT_EQ(p.full_attribute_name(), "object");
}

TEST_F(oh5_SplitPath_test, split_long_path_with_group) {
    SplitPath p("/dataset1/data1/what/object");
    EXPECT_EQ(p.group_path(), "/dataset1/data1/what");
    EXPECT_EQ(p.attribute_name(), "object");
    EXPECT_EQ(p.full_attribute_name(), "what/object");
}

TEST_F(oh5_SplitPath_test, split_short_path_with_group) {
    SplitPath p("/what/date");
    EXPECT_EQ(p.group_path(), "/what");
    EXPECT_EQ(p.attribute_name(), "date");
    EXPECT_EQ(p.full_attribute_name(), "what/date");
}
// END DEPRECATED TESTS

TEST_F(oh5_SplitPath_test, invalid_paths) {
    EXPECT_THROW(SplitPath("/an/invalid/path"), value_error);
}

//root::attribute
TEST_F(oh5_SplitPath_test, attribute) {
    SplitPath p("/Conventions");
    EXPECT_EQ("", p.dataset_group());
    EXPECT_EQ("", p.data_group());
    EXPECT_EQ("", p.quality_group());
    EXPECT_EQ("", p.attribute_group());
    EXPECT_EQ("Conventions", p.attribute());
}

//root::attributegroup::attribute
TEST_F(oh5_SplitPath_test, attributegroup_attribute) {
    SplitPath p("/what/object");
    EXPECT_EQ("", p.dataset_group());
    EXPECT_EQ("", p.data_group());
    EXPECT_EQ("", p.quality_group());
    EXPECT_EQ("what", p.attribute_group());
    EXPECT_EQ("object", p.attribute());
}

//root::dataset::attributegroup::attribute
TEST_F(oh5_SplitPath_test, dataset_attributegroup_attribute) {
    SplitPath p("/dataset1/what/object");
    EXPECT_EQ("dataset1", p.dataset_group());
    EXPECT_EQ("", p.data_group());
    EXPECT_EQ("", p.quality_group());
    EXPECT_EQ("what", p.attribute_group());
    EXPECT_EQ("object", p.attribute());
}

//root::dataset::data::attributegroup::attribute
TEST_F(oh5_SplitPath_test, dataset_data_attributegroup_attribute) {
    SplitPath p("/dataset1/data1/what/object");
    EXPECT_EQ("dataset1", p.dataset_group());
    EXPECT_EQ("data1", p.data_group());
    EXPECT_EQ("", p.quality_group());
    EXPECT_EQ("what", p.attribute_group());
    EXPECT_EQ("object", p.attribute());
}

//root::dataset::quality::attributegroup::attribute
TEST_F(oh5_SplitPath_test, dataset_quality_attributegroup_attribute) {
    SplitPath p("/dataset1/quality1/what/object");
    EXPECT_EQ("dataset1", p.dataset_group());
    EXPECT_EQ("", p.data_group());
    EXPECT_EQ("quality1", p.quality_group());
    EXPECT_EQ("what", p.attribute_group());
    EXPECT_EQ("object", p.attribute());
}

//root::dataset::data::quality::attributegroup::attribute
TEST_F(oh5_SplitPath_test, dataset_data_quality_attributegroup_attribute) {
    SplitPath p("/dataset1/data1/quality1/what/object");
    EXPECT_EQ("dataset1", p.dataset_group());
    EXPECT_EQ("data1", p.data_group());
    EXPECT_EQ("quality1", p.quality_group());
    EXPECT_EQ("what", p.attribute_group());
    EXPECT_EQ("object", p.attribute());
}

} // namespace oh5
} // namespace brfc
