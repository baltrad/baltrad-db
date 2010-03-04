#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataObject.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct AttributeGroup_test : public ::testing::Test {
    AttributeGroup_test()
            : g(make_shared<AttributeGroup>("g")) {
    }

    shared_ptr<AttributeGroup> g;
};

TEST_F(AttributeGroup_test, test_add_child_attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_NO_THROW(g->add_child(a));
}

TEST_F(AttributeGroup_test, test_add_child_attributegroup) {
    shared_ptr<AttributeGroup> g2 = make_shared<AttributeGroup>("g2");
    EXPECT_THROW(g->add_child(g2), value_error);
}

TEST_F(AttributeGroup_test, test_add_child_data_object) {
    shared_ptr<DataObject> d = make_shared<DataObject>("d");
    EXPECT_THROW(g->add_child(d), value_error);
}

} // namespace oh5
} // namespace brfc
