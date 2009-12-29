#include <boost/test/unit_test.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/File.hpp>
#include <brfc/SplitPath.hpp>
#include <brfc/DataObject.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "common.hpp"

using namespace brfc;

BOOST_AUTO_TEST_SUITE(File_test)

BOOST_AUTO_TEST_CASE(get_same_node) {
    File f;
    DataObject& d1 = f.data_object("/path/to/object", true);
    DataObject& d2 = f.data_object("/path/to/object", true);

    BOOST_CHECK_EQUAL(&d1, &d2);
}

BOOST_AUTO_TEST_CASE(get_nx_node) {
    File f;
    BOOST_CHECK_THROW(f.data_object("/nx"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(root) {
    File f;
    BOOST_CHECK_EQUAL(&f.root(), &f.data_object("/"));
}

BOOST_AUTO_TEST_CASE(split_short_path_without_group) {
    SplitPath p("/Conventions");
    BOOST_CHECK_EQUAL(p.data_object_path, "/");
    BOOST_CHECK_EQUAL(p.attribute_name, "Conventions");
}

BOOST_AUTO_TEST_CASE(split_long_path_without_group) {
    SplitPath p("/path/to/object");
    BOOST_CHECK_EQUAL(p.data_object_path, "/path/to");
    BOOST_CHECK_EQUAL(p.attribute_name, "object");
}

BOOST_AUTO_TEST_CASE(split_long_path_with_group) {
    SplitPath p("/path/to/what/object");
    BOOST_CHECK_EQUAL(p.data_object_path, "/path/to");
    BOOST_CHECK_EQUAL(p.attribute_name, "what/object");
}

BOOST_AUTO_TEST_CASE(split_short_path_with_group) {
    SplitPath p("/what/date");
    BOOST_CHECK_EQUAL(p.data_object_path, "/");
    BOOST_CHECK_EQUAL(p.attribute_name, "what/date");
}

BOOST_AUTO_TEST_CASE(path) {
    File f("pvol", "H5rad 2.0", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    BOOST_CHECK_EQUAL(f.path(), "pvol_20000102T120500.h5");
}

BOOST_AUTO_TEST_CASE(open_nx_file) {
    AttributeMapper m;
    BOOST_CHECK_THROW(File("/path/to/nxfile", m), fs_error);
}

BOOST_AUTO_TEST_CASE(read_write) {
    File f;
    AttributeMapper m;
    m.add_spec(0, "date", "date", "t", "v");
    m.add_spec(0, "time", "time", "t", "v");

    QVariant time(QTime(12, 5, 1));
    QVariant date(QDate(2000, 1, 2));
    DataObject& d = f.data_object("/dataset1", true);
    f.root().add_attribute("date", date);
    f.root().add_attribute("time", time);
    d.add_attribute("date", date);
    f.write("/tmp/asd", m);
    File g("/tmp/asd", m);
    BOOST_CHECK_EQUAL(f.root().attribute("date").value(), date);
    BOOST_CHECK_EQUAL(f.root().attribute("time").value(), time);
}

BOOST_AUTO_TEST_SUITE_END()
