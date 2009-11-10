#include <boost/test/unit_test.hpp>

#include <brfc/DataObject.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <algorithm>
#include <stdexcept>
#include <vector>

using namespace brfc;

BOOST_AUTO_TEST_SUITE(DataObject_test)

BOOST_AUTO_TEST_CASE(get_nx_child) {
    DataObject d("");
    BOOST_CHECK_THROW(d.child("nx"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(get_same_child) {
    DataObject d("");
    DataObject& d1 = d.child("bla", true);
    d.child("asd", true);
    DataObject& d2 = d.child("bla");
    BOOST_CHECK_EQUAL(&d1, &d2);
}

BOOST_AUTO_TEST_CASE(iterator) {
    DataObject a("a");
    DataObject& b = a.add_child("b");
    DataObject::iterator i = a.begin();
    BOOST_CHECK_EQUAL(&(*i), &a);
    ++i;
    BOOST_CHECK_EQUAL(&(*i), &b);
}

BOOST_AUTO_TEST_CASE(iterator_end) {
    DataObject a("a");
    DataObject::iterator i = a.begin();
    BOOST_CHECK(i != a.end());
    ++i;
    BOOST_CHECK(i == a.end());
}

BOOST_AUTO_TEST_CASE(iterate_tree) {
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

    BOOST_CHECK_EQUAL(name_str, expected);
}

BOOST_AUTO_TEST_CASE(path) {
    DataObject a("a");
    DataObject& b = a.add_child("b");
    DataObject& c = b.add_child("c");
    std::string expected = "a/b/c";

    BOOST_CHECK_EQUAL(c.path(), expected);
}

// compilational check
BOOST_AUTO_TEST_CASE(const_iterator) {
    DataObject a("a");
    DataObject::iterator i = a.begin(); 
    DataObject::const_iterator ci = a.begin();
    const DataObject& b = a;
    DataObject::const_iterator cci = b.begin();
}

BOOST_AUTO_TEST_SUITE_END()
