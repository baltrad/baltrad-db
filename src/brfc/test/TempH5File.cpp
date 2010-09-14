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

#include <brfc/test/TempH5File.hpp>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <brfc/visit.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Scalar.hpp>

namespace fs = boost::filesystem;

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
    typedef mpl::vector<const oh5::RootGroup,
                        const oh5::Group,
                        const oh5::Attribute> accepted_types;
    
    GatherHLNodes()
        : nodes_(HLNodeList_new(), &HLNodeList_free) {

    }

    void operator()(const oh5::RootGroup& root) {
        // pass, the file already has a root group by default
    }

    void operator()(const oh5::Group& group) {
        const String& path = group.path();

        // create node
        HL_Node* node = HLNode_newGroup(path.to_utf8().c_str());
        if (node == 0)
            throw std::runtime_error("could not create group node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0)
            throw std::runtime_error("could not add group node");
    }

    void operator()(const oh5::Attribute& attr) {
        const String& path = attr.path();

        // create node
        HL_Node* node = HLNode_newAttribute(path.to_utf8().c_str());
        if (node == 0)
            throw std::runtime_error("could not create attribute node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0) {
            throw std::runtime_error("could not add attribute node");
        }

        // convert and set value
        oh5::HL_Data d = convert(attr.value());
        HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
    }

    oh5::HL_Data
    convert(const oh5::Scalar& value) {
        switch (value.type()) {
            case oh5::Scalar::DOUBLE:
                return oh5::DoubleConverter().convert(value);
            case oh5::Scalar::INT64:
                return oh5::Int64Converter().convert(value);
            case oh5::Scalar::STRING:
                return oh5::StringConverter().convert(value);
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


String
TempH5File::path() const {
    return String::from_utf8(path_.get());
}

void
TempH5File::copy(const String& dest) const {
    fs::copy_file(path().to_utf8(), dest.to_utf8());
}

void
TempH5File::write(const oh5::File& file) {
    GatherHLNodes node_gather;

    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        visit(node, node_gather);
    }
    
    node_gather.write(path_.get());
}

} // namespace test
} // namespace brfc
