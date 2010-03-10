#include <gtest/gtest.h>

#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/FileLoader.hpp>
#include <brfc/oh5/Root.hpp>

#include <QtCore/QDate>
#include <QtCore/QTime>

#include "../common.hpp"
#include "../TempH5File.hpp"

namespace brfc {
namespace oh5 {

class oh5_FileLoader_test : public ::testing::Test {
  public:

    AttributeSpecs specs;
};


TEST_F(oh5_FileLoader_test, load) {
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

    shared_ptr<File> g = File::from_filesystem(f.filename(), specs);
    shared_ptr<Root> root = g->root();
    EXPECT_EQ((size_t)3, root->children().size());
    EXPECT_TRUE(root->has_child_by_name("date"));
    EXPECT_TRUE(root->has_child_by_name("time"));
    EXPECT_TRUE(root->has_child_by_name("dataset1"));
    ASSERT_TRUE(root->attribute("date"));
    ASSERT_TRUE(root->attribute("time"));
    ASSERT_TRUE(g->group("/dataset1"));
    ASSERT_TRUE(g->group("/dataset1")->attribute("date"));
    EXPECT_EQ(date, root->attribute("date")->value());
    EXPECT_EQ(time, root->attribute("time")->value());
    EXPECT_EQ(date, g->group("/dataset1")->attribute("date")->value());
}

TEST_F(oh5_FileLoader_test, ignored_attributes) {
    brfc::TempH5File f;
    f.add_attribute("/ignore", Variant(2.0));
    f.add_group("/dataset");
    f.add_attribute("/dataset/ignore", Variant(1.0));
    f.write();

    shared_ptr<File> g = File::from_filesystem(f.filename(), specs);
    EXPECT_FALSE(g->root()->attribute("ignore"));
    const File::StringVector& ignored = g->ignored_attributes();
    EXPECT_EQ(ignored.size(), (size_t)2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/dataset/ignore") != ignored.end());
}



} // namespace oh5
} // namespace brfc
