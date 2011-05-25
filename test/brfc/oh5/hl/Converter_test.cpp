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

#include <gtest/gtest.h>

#include <cstring>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/HlConverter.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

#include <brfc/test_common.hpp>

using boost::shared_ptr;
using boost::dynamic_pointer_cast;

namespace brfc {

namespace {

template<typename T>
shared_ptr<HL_Node>
create_hlhdf_attribute(const char* name, const char* tname, T value) {
    shared_ptr<HL_Node> node(HLNode_newAttribute(name), &HLNode_free);
    HLNode_setScalarValue(node.get(),
                          sizeof(T),
                          reinterpret_cast<unsigned char*>(&value),
                          tname,
                          -1);
    return node;
}

template<>
shared_ptr<HL_Node>
create_hlhdf_attribute(const char* name, const char* tname, const char* value) {
    shared_ptr<HL_Node> node(HLNode_newAttribute(name), &HLNode_free);
    HLNode_setScalarValue(node.get(),
                          strlen(value) + 1,
                          reinterpret_cast<unsigned char*>(const_cast<char*>(value)),
                          tname,
                          -1);
    return node;
}

} // namespace anonymous

TEST(oh5_hl_HlConverter_test, test_create_converter_from_hlhdf_node) {
    shared_ptr<HL_Node> node;
    shared_ptr<const HlConverter> conv;

    node = create_hlhdf_attribute("node", "int", (int)1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlInt64Converter>(conv));

    node = create_hlhdf_attribute("node", "long", (long)1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlInt64Converter>(conv));

    node = create_hlhdf_attribute("node", "llong", (long long)1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlInt64Converter>(conv));

    node = create_hlhdf_attribute("node", "float", (float)1.1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlDoubleConverter>(conv));

    node = create_hlhdf_attribute("node", "double", (double)1.1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlDoubleConverter>(conv));

    node = create_hlhdf_attribute("node", "ldouble", (long double)1.1);
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlDoubleConverter>(conv));

    node = create_hlhdf_attribute("node", "string", "foo");
    conv = HlConverter::create_from_hlhdf_node(*node);
    EXPECT_TRUE(dynamic_pointer_cast<const HlStringConverter>(conv));
}

TEST(oh5_hl_HlConverter_test, test_create_converter_from_variant) {
    shared_ptr<const HlConverter> conv;
    Oh5Scalar v(1);
    conv = HlConverter::create_from_variant(v);
    EXPECT_TRUE(dynamic_pointer_cast<const HlInt64Converter>(conv));

    v = Oh5Scalar(1.1);
    conv = HlConverter::create_from_variant(v);
    EXPECT_TRUE(dynamic_pointer_cast<const HlDoubleConverter>(conv));

    v = Oh5Scalar("foo");
    conv = HlConverter::create_from_variant(v);
    EXPECT_TRUE(dynamic_pointer_cast<const HlStringConverter>(conv));

}

TEST(oh5_HlInt64Converter_test, test_convert_hlnode) {
    HlInt64Converter conv;
    shared_ptr<HL_Node> node;
    Oh5Scalar v(0);
    
    node = create_hlhdf_attribute("node", "int", (int)1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar(1), v);

    node = create_hlhdf_attribute("node", "long", (long)1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar(1), v);

    node = create_hlhdf_attribute("node", "llong", (long long)1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar(1), v);

    node = create_hlhdf_attribute("node", "double", 1.1);
    EXPECT_THROW(conv.convert(*node), value_error);
    
    node = create_hlhdf_attribute("node", "string", "foo");
    EXPECT_THROW(conv.convert(*node), value_error);
}

TEST(oh5_HlInt64Converter_test, test_convert_variant) {
    HlInt64Converter conv;

    HL_Data d = conv.convert(Oh5Scalar(1));
    EXPECT_STREQ("llong", d.type());
    EXPECT_EQ(1, *reinterpret_cast<long long*>(d.data()));

    EXPECT_THROW(conv.convert(Oh5Scalar(1.1)), value_error);
    EXPECT_THROW(conv.convert(Oh5Scalar("asd")), value_error);
}

TEST(oh5_HlDoubleConverter_test, test_convert_hlnode) {
    HlDoubleConverter conv;
    shared_ptr<HL_Node> node;
    Oh5Scalar v(0);
    
    node = create_hlhdf_attribute("node", "float", (float)1.1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar((float)1.1), v);

    node = create_hlhdf_attribute("node", "double", (double)1.1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar(1.1), v);

    node = create_hlhdf_attribute("node", "ldouble", (long double)1.1);
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar(1.1), v);
    
    node = create_hlhdf_attribute("node", "llong", (long long)1);
    EXPECT_THROW(conv.convert(*node), value_error);
    
    node = create_hlhdf_attribute("node", "string", "foo");
    EXPECT_THROW(conv.convert(*node), value_error);
}

TEST(oh5_HlDoubleConverter_test, test_convert_variant) {
    HlDoubleConverter conv;

    HL_Data d = conv.convert(Oh5Scalar(1.1));
    EXPECT_STREQ("double", d.type());
    EXPECT_EQ(1.1, *reinterpret_cast<double*>(d.data()));

    EXPECT_THROW(conv.convert(Oh5Scalar(1)), value_error);
    EXPECT_THROW(conv.convert(Oh5Scalar("asd")), value_error);
}

TEST(oh5_HlStringConverter_test, test_convert_hlnode) {
    HlStringConverter conv;
    shared_ptr<HL_Node> node;
    Oh5Scalar v(0);
    
    node = create_hlhdf_attribute("node", "string", "foo");
    EXPECT_NO_THROW(v = conv.convert(*node)); 
    EXPECT_EQ(Oh5Scalar("foo"), v);

    node = create_hlhdf_attribute("node", "llong", (long long)1);
    EXPECT_THROW(conv.convert(*node), value_error);
    
    node = create_hlhdf_attribute("node", "double", 1.1);
    EXPECT_THROW(conv.convert(*node), value_error);
}

TEST(oh5_HlStringConverter_test, test_convert_variant) {
    HlStringConverter conv;

    HL_Data d = conv.convert(Oh5Scalar("foo"));
    EXPECT_STREQ("string", d.type());
    EXPECT_STREQ("foo", reinterpret_cast<const char*>(d.data()));

    EXPECT_THROW(conv.convert(Oh5Scalar(1)), value_error);
    EXPECT_THROW(conv.convert(Oh5Scalar(1.1)), value_error);
}


} // namespace brfc
