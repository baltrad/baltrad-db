/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/FileLoader.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/test/TempH5File.hpp>

#include <QtCore/QDate>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

class oh5_FileLoader_test : public ::testing::Test {
  public:
    oh5_FileLoader_test()
            : tempfile()
            , specs()
            , t_12_05_01(Time(12, 5, 1))
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
    shared_ptr<RootGroup> root = g->root();
    EXPECT_EQ((size_t)3, root->children().size());
    EXPECT_TRUE(root->has_child_by_name("date"));
    EXPECT_TRUE(root->has_child_by_name("time"));
    EXPECT_TRUE(root->has_child_by_name("what"));
    ASSERT_TRUE(root->child_attribute("date"));
    ASSERT_TRUE(root->child_attribute("time"));
    ASSERT_TRUE(g->group("/what"));
    ASSERT_TRUE(g->group("/what")->child_attribute("date"));
    EXPECT_EQ(d_2000_01_02, root->child_attribute("date")->value());
    EXPECT_EQ(t_12_05_01, root->child_attribute("time")->value());
    EXPECT_EQ(d_2000_01_02, g->group("/what")->child_attribute("date")->value());

    // nothing ignored
    EXPECT_EQ((size_t)0, g->invalid_attributes().size());
}

TEST_F(oh5_FileLoader_test, ignored_attributes) {
    shared_ptr<File> f = File::create();

    f->root()->add_child(make_shared<Attribute>("ignore", Variant(2.0)));

    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    what->add_child(make_shared<Attribute>("ignore", Variant(1.0)));
    f->root()->add_child(what);

    tempfile.write(*f);

    shared_ptr<File> g = File::from_filesystem(tempfile.path(), specs);

    shared_ptr<RootGroup> root = g->root();
    // still present in the structure
    EXPECT_TRUE(root->child_attribute("ignore"));
    EXPECT_TRUE(root->child_attribute("what/ignore"));

    // but accessible through ignored_attributes
    const File::StringVector& ignored = g->invalid_attribute_paths();
    EXPECT_EQ(ignored.size(), (size_t)2);
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/ignore") != ignored.end());
    EXPECT_TRUE(std::find(ignored.begin(), ignored.end(), "/what/ignore") != ignored.end());
}

TEST_F(oh5_FileLoader_test, invalid_conversion_throws) {
    shared_ptr<File> f = File::create();

    f->root()->add_child(make_shared<Attribute>("date", t_12_05_01));

    tempfile.write(*f);

    ASSERT_THROW(File::from_filesystem(tempfile.path(), specs), value_error);
}


} // namespace oh5
} // namespace brfc
