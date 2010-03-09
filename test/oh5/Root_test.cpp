#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Root.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_Root_test : public ::testing::Test {
    oh5_Root_test()
            : r(make_shared<Root>()) {
    }

    shared_ptr<Root> r;
};

TEST_F(oh5_Root_test, test_add_as_child) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_THROW(a->add_child(r), value_error);

    shared_ptr<AttributeGroup> ag = make_shared<AttributeGroup>("ag");
    EXPECT_THROW(ag->add_child(r), value_error);

    shared_ptr<Group> g = make_shared<Group>("g");
    EXPECT_THROW(g->add_child(r), value_error);

    shared_ptr<Root> r2 = make_shared<Root>();
    EXPECT_THROW(r2->add_child(r), value_error);
}

TEST_F(oh5_Root_test, test_file) {
    EXPECT_FALSE(r->file());
    shared_ptr<File> f = File::create();
    r->file(f);
    EXPECT_EQ(f, r->file());
}

// tests functionality implemented at Node level
TEST_F(oh5_Root_test, test_file_through_child_node) {
    shared_ptr<Group> g = make_shared<Group>("g");
    r->add_child(g);
    EXPECT_FALSE(g->file());
    shared_ptr<File> f = File::create();
    r->file(f);
    EXPECT_EQ(f, g->file());
}

} // namespace oh5
} // namespace brfc
