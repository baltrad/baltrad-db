#include <gtest/gtest.h>

#include <brfc/oh5/SplitPath.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

class oh5_SplitPath_test : public ::testing::Test {
  public:
    oh5_SplitPath_test()
            : attribute_group_names() {
    }

    virtual void SetUp() {
        attribute_group_names.append("what");
        attribute_group_names.append("where");
        attribute_group_names.append("how");
    }


    QStringList attribute_group_names;
};

TEST_F(oh5_SplitPath_test, split_short_path_without_group) {
    SplitPath p("/Conventions", attribute_group_names);
    EXPECT_EQ(p.group_path, "/");
    EXPECT_EQ(p.attribute_name, "Conventions");
    EXPECT_EQ(p.full_attribute_name, "Conventions");
}

TEST_F(oh5_SplitPath_test, split_long_path_without_group) {
    SplitPath p("/path/to/object", attribute_group_names);
    EXPECT_EQ(p.group_path, "/path/to");
    EXPECT_EQ(p.attribute_name, "object");
    EXPECT_EQ(p.full_attribute_name, "object");
}

TEST_F(oh5_SplitPath_test, split_long_path_with_group) {
    SplitPath p("/path/to/what/object", attribute_group_names);
    EXPECT_EQ(p.group_path, "/path/to/what");
    EXPECT_EQ(p.attribute_name, "object");
    EXPECT_EQ(p.full_attribute_name, "what/object");
}

TEST_F(oh5_SplitPath_test, split_short_path_with_group) {
    SplitPath p("/what/date", attribute_group_names);
    EXPECT_EQ(p.group_path, "/what");
    EXPECT_EQ(p.attribute_name, "date");
    EXPECT_EQ(p.full_attribute_name, "what/date");
}

} // namespace oh5
} // namespace brfc
