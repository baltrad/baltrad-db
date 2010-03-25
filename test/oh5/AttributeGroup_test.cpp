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

struct oh5_AttributeGroup_test : public ::testing::Test {
    oh5_AttributeGroup_test()
            : what(make_shared<AttributeGroup>("what")) {
    }

    shared_ptr<AttributeGroup> what;
};

TEST_F(oh5_AttributeGroup_test, test_add_child_Attribute) {
    shared_ptr<Attribute> a = make_shared<Attribute>("a", Variant(1));
    EXPECT_NO_THROW(what->add_child(a));
}

TEST_F(oh5_AttributeGroup_test, test_add_child_AttributeGroup) {
    shared_ptr<AttributeGroup> where = make_shared<AttributeGroup>("where");
    EXPECT_THROW(what->add_child(where), value_error);
}

TEST_F(oh5_AttributeGroup_test, test_add_child_DataGroup) {
    shared_ptr<DataGroup> data1 = make_shared<DataGroup>("data1");
    EXPECT_THROW(what->add_child(data1), value_error);
}

TEST_F(oh5_AttributeGroup_test, test_add_child_DataSetGroup) {
    shared_ptr<DataSetGroup> dataset2 = make_shared<DataSetGroup>("dataset2");
    EXPECT_THROW(what->add_child(dataset2), value_error);
}

TEST_F(oh5_AttributeGroup_test, test_add_child_QualityGroup) {
    shared_ptr<QualityGroup> quality1 = make_shared<QualityGroup>("quality1");
    EXPECT_THROW(what->add_child(quality1), value_error);
}

TEST_F(oh5_AttributeGroup_test, test_add_child_RootGroup) {
    shared_ptr<RootGroup> root = make_shared<RootGroup>();
    EXPECT_THROW(what->add_child(root), value_error);
}

} // namespace oh5
} // namespace brfc
