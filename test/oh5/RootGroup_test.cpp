#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/QualityGroup.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_RootGroup_test : public ::testing::Test {
    oh5_RootGroup_test()
            : root(make_shared<RootGroup>()) {
    }

    shared_ptr<RootGroup> root;
};

TEST_F(oh5_RootGroup_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_NO_THROW(root->add_child(a));
}

TEST_F(oh5_RootGroup_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    EXPECT_NO_THROW(root->add_child(what));
}

TEST_F(oh5_RootGroup_test, test_add_child_DataGroup) {
    shared_ptr<DataGroup> data1 = make_shared<DataGroup>("data1");
    EXPECT_THROW(root->add_child(data1), value_error);
}

TEST_F(oh5_RootGroup_test, test_add_child_DataSetGroup) {
    shared_ptr<DataSetGroup> dataset2 = make_shared<DataSetGroup>("dataset2");
    EXPECT_NO_THROW(root->add_child(dataset2));
}

TEST_F(oh5_RootGroup_test, test_add_child_QualityGroup) {
    shared_ptr<QualityGroup> quality1 = make_shared<QualityGroup>("quality1");
    EXPECT_THROW(root->add_child(quality1), value_error);
}

TEST_F(oh5_RootGroup_test, test_add_child_RootGroup) {
    shared_ptr<RootGroup> root2 = make_shared<RootGroup>();
    EXPECT_THROW(root->add_child(root2), value_error);
}

TEST_F(oh5_RootGroup_test, test_file) {
    EXPECT_FALSE(root->file());
    shared_ptr<File> f = File::create();
    root->file(f);
    EXPECT_EQ(f, root->file());
}

// tests functionality implemented at Node level
TEST_F(oh5_RootGroup_test, test_file_through_child_node) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    root->add_child(what);
    EXPECT_FALSE(what->file());
    shared_ptr<File> f = File::create();
    root->file(f);
    EXPECT_EQ(f, what->file());
}

} // namespace oh5
} // namespace brfc
