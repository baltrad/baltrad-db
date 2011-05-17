/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#include <vector>

#include <boost/foreach.hpp>

#include <gtest/gtest.h>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/MockNode.hpp>
#include <brfc/oh5/MockNodeBackend.hpp>

#include <brfc/test_common.hpp>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::ReturnNull;
using ::testing::ReturnRef;

namespace brfc {
namespace oh5 {


struct oh5_Oh5Node_test : public ::testing::Test {
    oh5_Oh5Node_test()
            : r("")
            , a("a")
            , f("f") 
            , be() {

    }

    virtual void SetUp() {
        r.backend(&be);
    }

    MockNode r, a, f;
    MockNodeBackend be;
};

TEST_F(oh5_Oh5Node_test, test_has_backend) {
    EXPECT_FALSE(a.has_backend());
    a.backend(&be);
    EXPECT_TRUE(a.has_backend());
}

TEST_F(oh5_Oh5Node_test, test_set_backend_twice) {
    a.backend(&be);
    EXPECT_THROW(a.backend(&be), std::runtime_error);
}

TEST_F(oh5_Oh5Node_test, test_backend_missing) {
    EXPECT_THROW(f.backend(), std::runtime_error);
}

TEST_F(oh5_Oh5Node_test, test_add) {
    EXPECT_CALL(be, do_add(&f))
        .WillOnce(ReturnRef(f));
     
    EXPECT_EQ(&f, &r.add(&f));
}

TEST_F(oh5_Oh5Node_test, test_has_child) {
    std::vector<const oh5::Oh5Node*> c;
    c.push_back(&a);

    EXPECT_CALL(be, do_children(Ref(r)))
        .WillOnce(Return(c))
        .WillOnce(Return(c));
    
    EXPECT_TRUE(r.has_child("a"));
    EXPECT_FALSE(r.has_child("f"));
}

TEST_F(oh5_Oh5Node_test, test_children) {
    std::vector<const oh5::Oh5Node*> c;
    c.push_back(&a);

    EXPECT_CALL(be, do_children(Ref(r)))
        .WillOnce(Return(c));

    std::vector<oh5::Oh5Node*> rc = r.children();
    ASSERT_EQ(1u, rc.size());
    EXPECT_EQ(&a, rc.at(0));
}


TEST_F(oh5_Oh5Node_test, test_path_root) {
    EXPECT_EQ("/", r.path());
}

TEST_F(oh5_Oh5Node_test, test_path) {
    a.parent(&r);
    f.parent(&a);

    EXPECT_EQ("/a/f", f.path());
}

TEST_F(oh5_Oh5Node_test, test_iterator_dereference) {
    Oh5Node::iterator i = r.begin();
    EXPECT_EQ(&r, &(*i));
}

TEST_F(oh5_Oh5Node_test, test_iterator_increment) {
    std::vector<const oh5::Oh5Node*> cr;
    std::vector<const oh5::Oh5Node*> ca;
    std::vector<const oh5::Oh5Node*> cf;
    a.backend(&be);
    f.backend(&be);
    cr.push_back(&a);
    cr.push_back(&f);

    {
        InSequence s;
        EXPECT_CALL(be, do_children(Ref(r)))
            .WillOnce(Return(cr));
        EXPECT_CALL(be, do_children(Ref(a)))
            .WillOnce(Return(ca));
        EXPECT_CALL(be, do_children(Ref(f)))
            .WillOnce(Return(cf));
    }
    
    Oh5Node::iterator i = r.begin();
    ++i;
    EXPECT_EQ("a", i->name());
    ++i;
    EXPECT_EQ("f", i->name());
    ++i;
}

TEST_F(oh5_Oh5Node_test, test_iterator_end) {
    std::vector<const oh5::Oh5Node*> c;
    EXPECT_CALL(be, do_children(Ref(r)))
        .WillOnce(Return(c));

    Oh5Node::iterator i = r.begin();
    EXPECT_TRUE(i != r.end());
    ++i;
    EXPECT_TRUE(i == r.end());
}

} // namespace oh5
} // namespace brfc
