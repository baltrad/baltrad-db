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

} // namespace oh5
} // namespace brfc
