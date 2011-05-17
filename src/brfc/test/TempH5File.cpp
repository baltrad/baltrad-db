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

#include <brfc/test/TempH5File.hpp>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

#include <brfc/visit.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/Converter.hpp>

#include <brfc/util/BoostFileSystem.hpp>

namespace brfc {
namespace test {

TempH5File::TempH5File()
        : path_(strdup("/tmp/brfctest_XXXXXX"), &free) {
    int fd = mkstemp(path_.get());
    if (fd == -1)
        throw std::runtime_error("could not create temporary file");
    close(fd);
}

TempH5File::~TempH5File() {
    unlink();
}

void
TempH5File::unlink() {
    ::unlink(path_.get());
}

namespace {

class GatherHLNodes {
  public:
    typedef mpl::vector<const oh5::Oh5DataSet,
                        const oh5::Oh5Group,
                        const oh5::Oh5Attribute> accepted_types;
    
    GatherHLNodes()
        : nodes_(HLNodeList_new(), &HLNodeList_free) {

    }

    void operator()(const oh5::Oh5DataSet& dataset) {
        const std::string& path = dataset.path();

        // create node
        HL_Node* node = HLNode_newDataset(path.c_str());
        if (node == 0)
            throw std::runtime_error("could not create dataset node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0)
            throw std::runtime_error("could not add dataset node");

        hsize_t dims[]={1};
        int array[] = {0};
        HLNode_setArrayValue(node, sizeof(int), 1, dims, (unsigned char*)array, "int", -1);
    }

    void operator()(const oh5::Oh5Group& group) {
        const std::string& path = group.path();

        // create node
        HL_Node* node = HLNode_newGroup(path.c_str());
        if (node == 0)
            throw std::runtime_error("could not create group node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0)
            throw std::runtime_error("could not add group node");
    }


    void operator()(const oh5::Oh5Attribute& attr) {
        const std::string& path = attr.path();

        // create node
        HL_Node* node = HLNode_newAttribute(path.c_str());
        if (node == 0)
            throw std::runtime_error("could not create attribute node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0) {
            throw std::runtime_error("could not add attribute node");
        }

        // convert and set value
        oh5::hl::HL_Data d = convert(attr.value());
        HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
    }

    oh5::hl::HL_Data
    convert(const oh5::Oh5Scalar& value) {
        switch (value.type()) {
            case oh5::Oh5Scalar::DOUBLE:
                return oh5::hl::DoubleConverter().convert(value);
            case oh5::Oh5Scalar::INT64:
                return oh5::hl::Int64Converter().convert(value);
            case oh5::Oh5Scalar::STRING:
                return oh5::hl::StringConverter().convert(value);
            default:
                throw std::runtime_error("could not convert");
        }
    }

    void write(const char* path) {
        HL_Compression compression;
        HLCompression_init(&compression, CT_ZLIB);
        compression.level = 6;
        if (HLNodeList_setFileName(nodes_.get(), path) == 0)
            throw std::runtime_error("could not set filename");
        if (HLNodeList_write(nodes_.get(), 0, &compression) == 0)
            throw std::runtime_error("could not write file");
    }

  private:
    shared_ptr<HL_NodeList> nodes_;
};

} // namespace anonymous


std::string
TempH5File::path() const {
    return std::string(path_.get());
}

void
TempH5File::copy(const std::string& dest) const {
    BoostFileSystem().copy_file(path(), dest);
}

void
TempH5File::write(const oh5::Oh5File& file) {
    GatherHLNodes node_gather;
    
    oh5::Oh5Node::const_iterator iter = file.root().begin();
    
    ++iter; // skip root
    for ( ; iter != file.root().end(); ++iter) {
        visit(*iter, node_gather);
    }

    node_gather.write(path_.get());
}

} // namespace test
} // namespace brfc
