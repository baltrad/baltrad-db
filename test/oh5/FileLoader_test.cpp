#include <gtest/gtest.h>

#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
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
        specs.add(AttributeSpec("what/date", "date"));
        specs.add(AttributeSpec("what/time", "time"));
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
    
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    what->add_child(make_shared<Attribute>("date", d_2000_01_02));
    f->root()->add_child(what);

    tempfile.write(*f);

    shared_ptr<File> g = File::from_filesystem(tempfile.path(), specs);
    EXPECT_EQ(g->path(), tempfile.path());
    shared_ptr<Root> root = g->root();
    EXPECT_EQ((size_t)3, root->children().size());
    EXPECT_TRUE(root->has_child_by_name("date"));
    EXPECT_TRUE(root->has_child_by_name("time"));
    EXPECT_TRUE(root->has_child_by_name("what"));
    ASSERT_TRUE(root->attribute("date"));
    ASSERT_TRUE(root->attribute("time"));
    ASSERT_TRUE(g->group("/what"));
    ASSERT_TRUE(g->group("/what")->attribute("date"));
    EXPECT_EQ(d_2000_01_02, root->attribute("date")->value());
    EXPECT_EQ(t_12_05_01, root->attribute("time")->value());
    EXPECT_EQ(d_2000_01_02, g->group("/what")->attribute("date")->value());

    // nothing ignored
    EXPECT_EQ(g->ignored_attributes().size(), 0);
}

TEST_F(oh5_FileLoader_test, ignored_attributes) {
    shared_ptr<File> f = File::create();

    f->root()->add_child(make_shared<Attribute>("ignore", Variant(2.0)));

    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    what->add_child(make_shared<Attribute>("ignore", Variant(1.0)));
    f->root()->add_child(what);

    tempfile.write(*f);

    shared_ptr<File> g = File::from_filesystem(tempfile.path(), specs);

    shared_ptr<Root> root = g->root();
    // still present in the structure
    EXPECT_TRUE(root->attribute("ignore"));
    EXPECT_TRUE(root->attribute("what/ignore"));

    // accessible through ignored_attributes
    const File::StringVector& ignored = g->ignored_attributes();
    EXPECT_EQ(ignored.size(), (size_t)2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/what/ignore") != ignored.end());
}



} // namespace oh5
} // namespace brfc
