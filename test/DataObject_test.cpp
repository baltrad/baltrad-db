#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Variant.hpp>

#include <stdexcept>

#include <QtCore/QStringList>

#include "common.hpp"

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
    QString expected = "abcedfg";

    QStringList names;

    BOOST_FOREACH(const DataObject& dobj, a) {
        names.append(dobj.name());
    }
    QString name_str = names.join("");

    EXPECT_EQ(name_str, expected);
}

TEST(DataObject_test, path) {
    DataObject a("a");
    DataObject& b = a.add_child("b");
    DataObject& c = b.add_child("c");
    QString expected("a/b/c");

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
