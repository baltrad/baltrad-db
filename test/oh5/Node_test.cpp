#include <gtest/gtest.h>

#include "../common.hpp"

#include <brfc/exceptions.hpp>
#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

struct Node_test : public ::testing::Test {
    Node_test()
            : a(make_shared<Node>("a"))
            , b(make_shared<Node>("b"))
            , c(make_shared<Node>("c"))
            , f(make_shared<Node>("f")) {
        a->add_child(b);
        b->add_child(c);
    }

    shared_ptr<Node> a, b, c, f;
};

TEST_F(Node_test, test_has_child) {
    EXPECT_TRUE(a->has_child(*b));
}

TEST_F(Node_test, test_has_child_by_name) {
    EXPECT_TRUE(a->has_child_by_name("b"));
}

TEST_F(Node_test, test_add_child) {
    a->add_child(f);
    EXPECT_TRUE(a->has_child(*f));
    EXPECT_EQ(f->parent(), a);
}

TEST_F(Node_test, remove_child) {
    a->remove_child(*b);
    EXPECT_FALSE(a->has_child(*b));
    EXPECT_TRUE(b->is_root());
}

TEST_F(Node_test, test_add_child_null_pointer) {
    EXPECT_THROW(a->add_child(shared_ptr<Node>()), value_error);
}

TEST_F(Node_test, test_parent) {
    EXPECT_EQ(b->parent(), a);
}

TEST_F(Node_test, test_reparent) {
    a->add_child(c);
    EXPECT_TRUE(a->has_child(*c));
    EXPECT_EQ(c->parent(), a);
    EXPECT_FALSE(b->has_child(*c));
}

TEST_F(Node_test, path) {
    EXPECT_EQ(a->path(), "/a");
    EXPECT_EQ(c->path(), "/a/b/c");
}

TEST_F(Node_test, throws_on_duplicate_child) {
    EXPECT_THROW(a->add_child(b), duplicate_entry);
}

TEST_F(Node_test, changing_child_name_throws_when_parent_has_entry) {
    a->add_child(f);
    EXPECT_THROW(f->name("b"), duplicate_entry);
}

TEST_F(Node_test, is_root) {
    EXPECT_TRUE(a->is_root());
    EXPECT_FALSE(b->is_root());
}

} // namespace oh5
} // namespace brfc
