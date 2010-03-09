#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Group_test : public ::testing::Test {
    oh5_Group_test()
            : g(make_shared<Group>("g")) {
    }

    shared_ptr<Group> g;
};

TEST_F(oh5_Group_test, test_add_child_attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_NO_THROW(g->add_child(a));
}

TEST_F(oh5_Group_test, test_add_child_attributegroup) {
    shared_ptr<AttributeGroup> g2 = make_shared<AttributeGroup>("g2");
    EXPECT_NO_THROW(g->add_child(g2));
}

TEST_F(oh5_Group_test, test_add_child_group) {
    shared_ptr<Group> d = make_shared<Group>("d");
    EXPECT_NO_THROW(g->add_child(d));
}

TEST_F(oh5_Group_test, test_attribute_access) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    g->add_child(what);
    shared_ptr<Attribute> a1 = make_shared<Attribute>("a1", Variant(1));
    what->add_child(a1);

    EXPECT_EQ(g->attribute("what/a1"), a1);
    EXPECT_FALSE(g->attribute("a1"));
    EXPECT_FALSE(g->attribute("what"));
}

TEST_F(oh5_Group_test, test_group_by_name) {
    shared_ptr<Group> g2 = make_shared<Group>("g2");
    g->add_child(g2);
    shared_ptr<AttributeGroup> ag = make_shared<AttributeGroup>("ag");
    g2->add_child(ag);
    
    EXPECT_EQ(g->group_by_name("g2"), g2);
    EXPECT_EQ(g2->group_by_name("ag"), ag);
    EXPECT_FALSE(g->group_by_name("ag"));
}

} // namespace oh5
} // namespace brfc
