#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Data.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Quality.hpp>
#include <brfc/oh5/Root.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Root_test : public ::testing::Test {
    oh5_Root_test()
            : root(make_shared<Root>()) {
    }

    shared_ptr<Root> root;
};

TEST_F(oh5_Root_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_NO_THROW(root->add_child(a));
}

TEST_F(oh5_Root_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    EXPECT_NO_THROW(root->add_child(what));
}

TEST_F(oh5_Root_test, test_add_child_Data) {
    shared_ptr<Data> data1 = make_shared<Data>("data1");
    EXPECT_THROW(root->add_child(data1), value_error);
}

TEST_F(oh5_Root_test, test_add_child_DataSet) {
    shared_ptr<DataSet> dataset2 = make_shared<DataSet>("dataset2");
    EXPECT_NO_THROW(root->add_child(dataset2));
}

TEST_F(oh5_Root_test, test_add_child_Quality) {
    shared_ptr<Quality> quality1 = make_shared<Quality>("quality1");
    EXPECT_THROW(root->add_child(quality1), value_error);
}

TEST_F(oh5_Root_test, test_add_child_Root) {
    shared_ptr<Root> root2 = make_shared<Root>();
    EXPECT_THROW(root->add_child(root2), value_error);
}

TEST_F(oh5_Root_test, test_file) {
    EXPECT_FALSE(root->file());
    shared_ptr<File> f = File::create();
    root->file(f);
    EXPECT_EQ(f, root->file());
}

// tests functionality implemented at Node level
TEST_F(oh5_Root_test, test_file_through_child_node) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    root->add_child(what);
    EXPECT_FALSE(what->file());
    shared_ptr<File> f = File::create();
    root->file(f);
    EXPECT_EQ(f, what->file());
}

} // namespace oh5
} // namespace brfc
