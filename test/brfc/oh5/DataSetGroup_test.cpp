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

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>
#include <brfc/oh5/QualityGroup.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include "../common.hpp"

namespace brfc {
namespace oh5 {

struct oh5_DataSetGroup_test : public ::testing::Test {
    oh5_DataSetGroup_test()
            : dataset1(make_shared<DataSetGroup>("dataset1")) {
    }

    shared_ptr<DataSetGroup> dataset1;
};

TEST_F(oh5_DataSetGroup_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_THROW(dataset1->add_child(a), value_error);
}

TEST_F(oh5_DataSetGroup_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    EXPECT_NO_THROW(dataset1->add_child(what));
}

TEST_F(oh5_DataSetGroup_test, test_add_child_DataGroup) {
    shared_ptr<DataGroup> data1 = make_shared<DataGroup>("data1");
    EXPECT_NO_THROW(dataset1->add_child(data1));
}

TEST_F(oh5_DataSetGroup_test, test_add_child_DataSetGroup) {
    shared_ptr<DataSetGroup> dataset2 = make_shared<DataSetGroup>("dataset2");
    EXPECT_THROW(dataset1->add_child(dataset2), value_error);
}

TEST_F(oh5_DataSetGroup_test, test_add_child_QualityGroup) {
    shared_ptr<QualityGroup> quality1 = make_shared<QualityGroup>("quality1");
    EXPECT_NO_THROW(dataset1->add_child(quality1));
}

TEST_F(oh5_DataSetGroup_test, test_add_child_RootGroup) {
    shared_ptr<RootGroup> root = make_shared<RootGroup>();
    EXPECT_THROW(dataset1->add_child(root), value_error);
}

} // namespace oh5
} // namespace brfc
