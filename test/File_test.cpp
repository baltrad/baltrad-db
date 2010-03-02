#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/File.hpp>
#include <brfc/SourceRadar.hpp>
#include <brfc/SplitPath.hpp>
#include <brfc/Variant.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include "common.hpp"
#include "TempH5File.hpp"

namespace brfc {

struct File_test : public testing::Test {
    File_test()
            : specs()
            , src(new SourceRadar()) {

    }

    virtual void SetUp() {
        src->node_id("seang");
    }

    AttributeSpecs specs;
    shared_ptr<SourceRadar> src;
};

TEST_F(File_test, get_same_node) {
    File f;
    DataObject& d1 = f.data_object("/path/to/object", true);
    DataObject& d2 = f.data_object("/path/to/object", true);

    EXPECT_EQ(&d1, &d2);
}

TEST_F(File_test, get_nx_node) {
    File f;
    EXPECT_THROW(f.data_object("/nx"), std::runtime_error);
}

TEST_F(File_test, root) {
    File f;
    EXPECT_EQ(&f.root(), &f.data_object("/"));
}

TEST_F(File_test, split_short_path_without_group) {
    SplitPath p("/Conventions");
    EXPECT_EQ(p.data_object_path, "/");
    EXPECT_EQ(p.attribute_name, "Conventions");
}

TEST_F(File_test, split_long_path_without_group) {
    SplitPath p("/path/to/object");
    EXPECT_EQ(p.data_object_path, "/path/to");
    EXPECT_EQ(p.attribute_name, "object");
}

TEST_F(File_test, split_long_path_with_group) {
    SplitPath p("/path/to/what/object");
    EXPECT_EQ(p.data_object_path, "/path/to");
    EXPECT_EQ(p.attribute_name, "what/object");
}

TEST_F(File_test, split_short_path_with_group) {
    SplitPath p("/what/date");
    EXPECT_EQ(p.data_object_path, "/");
    EXPECT_EQ(p.attribute_name, "what/date");
}

TEST_F(File_test, unique_identifier_same_meta) {
    File f1("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    File f2("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    f1.source(src);
    f2.source(src);
    EXPECT_EQ(f1.unique_identifier(), f2.unique_identifier());
}

TEST_F(File_test, unique_identifier_different_meta) {
    File f1("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    File f2("pvol", QDate(2001, 1, 2), QTime(12, 5), "WMO:02606");
    f1.source(src);
    f2.source(src);
    EXPECT_NE(f1.unique_identifier(), f2.unique_identifier());
}

TEST_F(File_test, unique_identifier_ignore_attribute) {
    File f("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    f.source(src);
    QString uid = f.unique_identifier();
    f.root().add_attribute("bla", Variant(1), true);
    EXPECT_EQ(uid, f.unique_identifier());
}

TEST_F(File_test, unique_identifier_changes_when_meta_changes) {
    File f("pvol", QDate(2000, 1, 2), QTime(12, 5), "WMO:02606");
    f.source(src);
    QString uid = f.unique_identifier();
    f.root().add_attribute("bla", Variant(1), false);
    EXPECT_NE(uid, f.unique_identifier());
}

TEST_F(File_test, open_nx_file) {
    EXPECT_THROW(File("/path/to/nxfile", specs), fs_error);
}

TEST_F(File_test, read) {
    specs.add(AttributeSpec("date", "date"));
    specs.add(AttributeSpec("time", "time"));

    Variant time(QTime(12, 5, 1));
    Variant date(QDate(2000, 1, 2));

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

TEST_F(File_test, ignored_attributes) {
    brfc::TempH5File f;
    f.add_attribute("/ignore", Variant(2.0));
    f.add_group("/dataset");
    f.add_attribute("/dataset/ignore", Variant(1.0));
    f.write();

    File g(f.filename(), specs);
    EXPECT_THROW(g.root().attribute("ignore"), brfc::lookup_error);
    const File::StringVector& ignored = g.ignored_attributes();
    EXPECT_EQ(ignored.size(), (size_t)2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/dataset/ignore") != ignored.end());
}

} // namespace brfc
