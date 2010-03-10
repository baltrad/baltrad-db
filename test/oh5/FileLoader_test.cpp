#include <gtest/gtest.h>

#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/FileLoader.hpp>
#include <brfc/oh5/Root.hpp>
#include <brfc/test/TempH5File.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

class oh5_FileLoader_test : public ::testing::Test {
  public:
    oh5_FileLoader_test()
            : tempfile()
            , specs()
            , t_12_05_01(QTime(12, 5, 1))
            , d_2000_01_02(QDate(2000, 1, 2)) {
    }

    virtual void SetUp() {
        specs.add(AttributeSpec("date", "date"));
        specs.add(AttributeSpec("time", "time"));
    }

    virtual void TearDown() {
        specs.clear();
    }

    test::TempH5File tempfile;
    AttributeSpecs specs;
    Variant t_12_05_01;
    Variant d_2000_01_02;
};


TEST_F(oh5_FileLoader_test, load) {
    shared_ptr<File> f = File::create();

    f->root()->add_child(make_shared<Attribute>("date", d_2000_01_02));
    f->root()->add_child(make_shared<Attribute>("time", t_12_05_01));
    
    shared_ptr<Group> ds1 = make_shared<Group>("dataset1");
    ds1->add_child(make_shared<Attribute>("date", d_2000_01_02));
    f->root()->add_child(ds1);

    tempfile.write(*f);

    shared_ptr<File> g = File::from_filesystem(tempfile.filename(), specs);
    shared_ptr<Root> root = g->root();
    EXPECT_EQ((size_t)3, root->children().size());
    EXPECT_TRUE(root->has_child_by_name("date"));
    EXPECT_TRUE(root->has_child_by_name("time"));
    EXPECT_TRUE(root->has_child_by_name("dataset1"));
    ASSERT_TRUE(root->attribute("date"));
    ASSERT_TRUE(root->attribute("time"));
    ASSERT_TRUE(g->group("/dataset1"));
    ASSERT_TRUE(g->group("/dataset1")->attribute("date"));
    EXPECT_EQ(d_2000_01_02, root->attribute("date")->value());
    EXPECT_EQ(t_12_05_01, root->attribute("time")->value());
    EXPECT_EQ(d_2000_01_02, g->group("/dataset1")->attribute("date")->value());
}

TEST_F(oh5_FileLoader_test, ignored_attributes) {
    shared_ptr<File> f = File::create();

    f->root()->add_child(make_shared<Attribute>("ignore", Variant(2.0)));

    shared_ptr<Group> ds = make_shared<Group>("dataset");
    ds->add_child(make_shared<Attribute>("ignore", Variant(1.0)));
    f->root()->add_child(ds);

    tempfile.write(*f);

    shared_ptr<File> g = File::from_filesystem(tempfile.filename(), specs);

    shared_ptr<Root> root = g->root();
    EXPECT_FALSE(root->attribute("ignore"));
    EXPECT_FALSE(root->attribute("dataset/ignore"));

    const File::StringVector& ignored = g->ignored_attributes();
    EXPECT_EQ(ignored.size(), (size_t)2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/dataset/ignore") != ignored.end());
}



} // namespace oh5
} // namespace brfc
