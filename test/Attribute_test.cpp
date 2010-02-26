#include <gtest/gtest.h>

#include <brfc/Attribute.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Variant.hpp>

#include "common.hpp"

namespace brfc {

TEST(Attribute_test, test_path_with_dobj) {
    DataObject dobj("dataset1");
    Attribute attr("attr1", Variant("bla"), &dobj);
    EXPECT_EQ(attr.path(), "/dataset1/attr1");
}


TEST(Attribute_test, test_path_without_dobj) {
    Attribute attr("attr1", Variant("bla"), 0);
    EXPECT_EQ(attr.path(), "attr1");
}

TEST(Attribute_test, test_to_string) {
    DataObject dobj("dataset1");
    Attribute attr("attr1", Variant("bla"), &dobj);
    EXPECT_EQ(attr.to_string(), "/dataset1/attr1=bla");
}

} // namespace brfc
