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

#include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

#include <brfc/visit.hpp>
#include <brfc/oh5/Oh5Attribute.hpp>
#include <brfc/oh5/Oh5DataSet.hpp>
#include <brfc/oh5/Oh5File.hpp>
#include <brfc/oh5/Oh5Group.hpp>
#include <brfc/oh5/Oh5Metadata.hpp>
#include <brfc/oh5/Oh5Scalar.hpp>

#include <brfc/oh5/hl/hlhdf.hpp>
#include <brfc/oh5/hl/HlConverter.hpp>

namespace brfc {

namespace {

class GatherHLNodes {
  public:
    typedef mpl::vector<const Oh5DataSet,
                        const Oh5Group,
                        const Oh5Attribute> accepted_types;
    
    GatherHLNodes()
        : nodes_(HLNodeList_new(), &HLNodeList_free) {

    }

    void operator()(const Oh5DataSet& dataset) {
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

    void operator()(const Oh5Group& group) {
        const std::string& path = group.path();

        // create node
        HL_Node* node = HLNode_newGroup(path.c_str());
        if (node == 0)
            throw std::runtime_error("could not create group node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0)
            throw std::runtime_error("could not add group node");
    }


    void operator()(const Oh5Attribute& attr) {
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
        HL_Data d = convert(attr.value());
        HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
    }

    HL_Data
    convert(const Oh5Scalar& value) {
        switch (value.type()) {
            case Oh5Scalar::DOUBLE:
                return HlDoubleConverter().convert(value);
            case Oh5Scalar::INT64:
                return HlInt64Converter().convert(value);
            case Oh5Scalar::STRING:
                return HlStringConverter().convert(value);
            default:
                throw std::runtime_error("could not convert");
        }
    }

    void write(const std::string& path) {
        HL_Compression compression;
        HLCompression_init(&compression, CT_ZLIB);
        compression.level = 6;
        if (HLNodeList_setFileName(nodes_.get(), path.c_str()) == 0)
            throw std::runtime_error("could not set filename");
        if (HLNodeList_write(nodes_.get(), 0, &compression) == 0)
            throw std::runtime_error("could not write file");
    }

  private:
    boost::shared_ptr<HL_NodeList> nodes_;
};

} // namespace anonymous

void
Oh5HlFileWriter::do_write(const Oh5File& from, const std::string& to) const {
    GatherHLNodes node_gather;

    const Oh5Node& root = from.metadata().root();
    
    Oh5Node::const_iterator iter = root.begin();
    
    ++iter; // skip root
    for ( ; iter != root.end(); ++iter) {
        visit(*iter, node_gather);
    }

    node_gather.write(to);
}

} // namespace brfc
