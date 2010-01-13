#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/File.hpp>
#include <brfc/SplitPath.hpp>
#include <brfc/DataObject.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QVariant>

#include "common.hpp"
#include "TempH5File.hpp"

using namespace brfc;

TEST(File_test, get_same_node) {
    File f;
    DataObject& d1 = f.data_object("/path/to/object", true);
    DataObject& d2 = f.data_object("/path/to/object", true);

    EXPECT_EQ(&d1, &d2);
}

TEST(File_test, get_nx_node) {
    File f;
    EXPECT_THROW(f.data_object("/nx"), std::runtime_error);
}

TEST(File_test, root) {
    File f;
    EXPECT_EQ(&f.root(), &f.data_object("/"));
}

TEST(File_test, split_short_path_without_group) {
    SplitPath p("/Conventions");
    EXPECT_EQ(p.data_object_path, "/");
    EXPECT_EQ(p.attribute_name, "Conventions");
}

TEST(File_test, split_long_path_without_group) {
    SplitPath p("/path/to/object");
    EXPECT_EQ(p.data_object_path, "/path/to");
    EXPECT_EQ(p.attribute_name, "object");
}

TEST(File_test, split_long_path_with_group) {
    SplitPath p("/path/to/what/object");
    EXPECT_EQ(p.data_object_path, "/path/to");
    EXPECT_EQ(p.attribute_name, "what/object");
}

TEST(File_test, split_short_path_with_group) {
    SplitPath p("/what/date");
    EXPECT_EQ(p.data_object_path, "/");
    EXPECT_EQ(p.attribute_name, "what/date");
}

TEST(File_test, unique_identifier) {
    File f("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    EXPECT_EQ(f.unique_identifier(), "pvol_20000102T120500_WMO:02606");
}

TEST(File_test, open_nx_file) {
    AttributeSpecs specs;
    EXPECT_THROW(File("/path/to/nxfile", specs), fs_error);
}

TEST(File_test, read) {
    AttributeSpecs specs;
    specs.add("date", "date");
    specs.add("time", "time");

    QVariant time(QTime(12, 5, 1));
    QVariant date(QDate(2000, 1, 2));

    brfc::TempH5File f;
    f.add_attribute("/date", date);
    f.add_attribute("/time", time);
    f.add_group("/dataset1");
    f.add_attribute("/dataset1/date", date);
    f.write();

    File g(f.filename(), specs);
    EXPECT_EQ(g.root().attribute("date").value(), date);
    EXPECT_EQ(g.root().attribute("time").value(), time);
    EXPECT_EQ(g.data_object("/dataset1").attribute("date").value(), date);
}

TEST(File_test, ignored_attributes) {
    AttributeSpecs specs;

    brfc::TempH5File f;
    f.add_attribute("/ignore", QVariant(2.0));
    f.add_group("/dataset");
    f.add_attribute("/dataset/ignore", QVariant(1.0));
    f.write();

    File g(f.filename(), specs);
    EXPECT_THROW(g.root().attribute("ignore"), brfc::lookup_error);
    const File::StringVector& ignored = g.ignored_attributes();
    EXPECT_EQ(ignored.size(), 2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/dataset/ignore") != ignored.end());
}
