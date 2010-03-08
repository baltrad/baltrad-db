#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Attribute_test : public ::testing::Test {
  public:
    oh5_Attribute_test()
            : a1(make_shared<Attribute>("a1", Variant(1)))
            , a2(make_shared<Attribute>("a2", Variant(2)))
            , g1(make_shared<AttributeGroup>("g1"))
            , d1(make_shared<Group>("d1")) {

    }
    
    shared_ptr<Attribute> a1;
    shared_ptr<Attribute> a2;
    shared_ptr<AttributeGroup> g1;
    shared_ptr<Group> d1;
};

TEST_F(oh5_Attribute_test, test_full_name) {
    EXPECT_EQ(a1->full_name(), "a1");
    d1->add_child(a1);
    EXPECT_EQ(a1->full_name(), "a1");
    g1->add_child(a2);
    EXPECT_EQ(a2->full_name(), "g1/a2");
}

TEST_F(oh5_Attribute_test, add_child) {
    EXPECT_THROW(a1->add_child(g1), value_error);
    EXPECT_THROW(a1->add_child(d1), value_error);
}

TEST_F(oh5_Attribute_test, to_string) {
    EXPECT_EQ(a1->to_string(), "/a1=1");
    d1->add_child(g1);
    g1->add_child(a2);
    EXPECT_EQ(a2->to_string(), "/d1/g1/a2=2");
}


} // namepsace oh5
} // namespace brfc
