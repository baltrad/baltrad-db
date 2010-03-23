#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Data.hpp>
#include <brfc/oh5/DataSet.hpp>
#include <brfc/oh5/Quality.hpp>
#include <brfc/oh5/Root.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Attribute_test : public ::testing::Test {
  public:
    oh5_Attribute_test()
            : a1(make_shared<Attribute>("a1", Variant(1)))
            , a2(make_shared<Attribute>("a2", Variant(2)))
            , what(make_shared<AttributeGroup>("what"))
            , dataset1(make_shared<DataSet>("dataset1"))
            , root(make_shared<Root>()) {

    }
    
    shared_ptr<Attribute> a1;
    shared_ptr<Attribute> a2;
    shared_ptr<AttributeGroup> what;
    shared_ptr<DataSet> dataset1;
    shared_ptr<Root> root;
};

TEST_F(oh5_Attribute_test, test_full_name) {
    EXPECT_EQ(a1->full_name(), "a1");
    root->add_child(a1);
    EXPECT_EQ(a1->full_name(), "a1");
    what->add_child(a2);
    EXPECT_EQ(a2->full_name(), "what/a2");
}

TEST_F(oh5_Attribute_test, to_string) {
    EXPECT_EQ(a1->to_string(), "/a1=1");
    root->add_child(dataset1);
    dataset1->add_child(what);
    what->add_child(a2);
    EXPECT_EQ(a2->to_string(), "/dataset1/what/a2=2");
}

TEST_F(oh5_Attribute_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_THROW(a1->add_child(a), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    EXPECT_THROW(a1->add_child(what), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_Data) {
    shared_ptr<Data> data1 = make_shared<Data>("data1");
    EXPECT_THROW(a1->add_child(data1), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_DataSet) {
    shared_ptr<DataSet> dataset2 = make_shared<DataSet>("dataset2");
    EXPECT_THROW(a1->add_child(dataset2), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_Quality) {
    shared_ptr<Quality> a1 = make_shared<Quality>("a1");
    EXPECT_THROW(a1->add_child(a1), value_error);
}

TEST_F(oh5_Attribute_test, test_add_child_Root) {
    shared_ptr<Root> root = make_shared<Root>();
    EXPECT_THROW(a1->add_child(root), value_error);
}

} // namepsace oh5
} // namespace brfc
