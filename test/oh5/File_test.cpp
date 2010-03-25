#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_File_test : public testing::Test {
    oh5_File_test()
            : specs()
            , src(new SourceRadar())
            , f1(File::minimal("pvol",
                               QDate(2000, 1, 2),
                               QTime(12, 5),
                               "WMO:02606"))
            , f2(File::minimal("pvol",
                               QDate(2001, 1, 2),
                               QTime(12, 5),
                               "WMO:02606"))
            // same as f1
            , f3(File::minimal("pvol",
                               QDate(2000, 1, 2),
                               QTime(12, 5),
                               "WMO:02606")) {
    }

    virtual void SetUp() {
        src->node_id("seang");
    }

    AttributeSpecs specs;
    shared_ptr<SourceRadar> src;
    shared_ptr<File> f1, f2, f3;
};

/*
TEST_F(oh5_File_test, get_same_node) {
    File f;
    DataObject& d1 = f.data_object("/path/to/object", true);
    DataObject& d2 = f.data_object("/path/to/object", true);

    EXPECT_EQ(&d1, &d2);
}
*/

TEST_F(oh5_File_test, get_nx_node) {
    shared_ptr<File> f = File::create();
    EXPECT_FALSE(f->group("/nx"));
}

TEST_F(oh5_File_test, root) {
    shared_ptr<File> f = File::create();
    EXPECT_EQ(f->root(), f->group("/"));
    EXPECT_EQ(f, f->root()->file());
}

TEST_F(oh5_File_test, required_attribute_shortcuts) {
    EXPECT_EQ(f1->what_object(), "pvol");
    EXPECT_EQ(f1->what_date(), QDate(2000, 1, 2));
    EXPECT_EQ(f1->what_time(), QTime(12, 5));
    EXPECT_EQ(f1->what_source(), "WMO:02606");
}

TEST_F(oh5_File_test, unique_identifier_same_meta) {
    f1->source(src);
    f3->source(src);
    EXPECT_EQ(f1->unique_identifier(), f3->unique_identifier());
}

TEST_F(oh5_File_test, unique_identifier_different_meta) {
    f1->source(src);
    f2->source(src);
    EXPECT_NE(f1->unique_identifier(), f2->unique_identifier());
}

TEST_F(oh5_File_test, unique_identifier_ignore_attribute) {
    f1->source(src);
    QString uid = f1->unique_identifier();
    f1->root()->add_child(make_shared<Attribute>("bla", Variant(1), true));
    EXPECT_EQ(uid, f1->unique_identifier());
}

TEST_F(oh5_File_test, unique_identifier_changes_when_meta_changes) {
    f1->source(src);
    QString uid = f1->unique_identifier();
    f1->root()->add_child(make_shared<Attribute>("bla", Variant(1), false));
    EXPECT_NE(uid, f1->unique_identifier());
}

TEST_F(oh5_File_test, open_nx_file) {
    EXPECT_THROW(File::from_filesystem("/path/to/nxfile", specs), fs_error);
}

} // namespace oh5
} // namespace brfc
