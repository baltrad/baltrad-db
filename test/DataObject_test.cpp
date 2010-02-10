#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Variant.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace brfc;

TEST(DataObject_test, get_nx_child) {
    DataObject d("");
    EXPECT_THROW(d.child("nx"), std::runtime_error);
}

TEST(DataObject_test, get_same_child) {
    DataObject d("");
    DataObject& d1 = d.child("bla", true);
    d.child("asd", true);
    DataObject& d2 = d.child("bla");
    EXPECT_EQ(&d1, &d2);
}

TEST(DataObject_test, iterator) {
    DataObject a("a");
    DataObject& b = a.add_child("b");
    DataObject::iterator i = a.begin();
    EXPECT_EQ(&(*i), &a);
    ++i;
    EXPECT_EQ(&(*i), &b);
}

TEST(DataObject_test, iterator_end) {
    DataObject a("a");
    DataObject::iterator i = a.begin();
    EXPECT_TRUE(i != a.end());
    ++i;
    EXPECT_TRUE(i == a.end());
}

TEST(DataObject_test, iterate_tree) {
    DataObject a("a");
    a.add_child("b");
    DataObject& c = a.add_child("c");
    DataObject& d = c.add_child("d");
    DataObject& e = a.add_child("e");
    e.add_child("f");
    d.add_child("g");
    std::string expected = "abcedfg";

    std::vector<std::string> names(7);

    using namespace boost::lambda;
    std::transform(a.begin(), a.end(), names.begin(),
                   bind(&DataObject::name, _1));
    std::string name_str = boost::join(names, "");

    EXPECT_EQ(name_str, expected);
}

TEST(DataObject_test, path) {
    DataObject a("a");
    DataObject& b = a.add_child("b");
    DataObject& c = b.add_child("c");
    std::string expected = "a/b/c";

    EXPECT_EQ(c.path(), expected);
}

// compilational check
TEST(DataObject_test, const_iterator) {
    DataObject a("a");
    DataObject::iterator i = a.begin(); 
    DataObject::const_iterator ci = a.begin();
    const DataObject& b = a;
    DataObject::const_iterator cci = b.begin();
}

TEST(DataObject_test, add_attribute_twice) {
    DataObject a("a");
    a.add_attribute("asd", Variant(1));
    EXPECT_THROW(a.add_attribute("asd", Variant(1)), duplicate_entry);
}
