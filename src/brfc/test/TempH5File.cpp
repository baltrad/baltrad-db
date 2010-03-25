#include <brfc/test/TempH5File.hpp>

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <unistd.h>

#include <boost/foreach.hpp>

#include <brfc/visit.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/Converter.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

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
        const QString& path = group.path();

        // create node
        HL_Node* node = HLNode_newGroup(path.toAscii().constData());
        if (node == 0)
            throw std::runtime_error("could not create group node");
        
        // add node to nodelist
        if (HLNodeList_addNode(nodes_.get(), node) == 0)
            throw std::runtime_error("could not add group node");
    }

    void operator()(const oh5::Attribute& attr) {
        const QString& path = attr.path();

        // create node
        HL_Node* node = HLNode_newAttribute(path.toAscii().constData());
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
    convert(const Variant& value) {
        switch (value.type()) {
            case Variant::DOUBLE:
                return oh5::RealConverter().convert(value);
            case Variant::LONGLONG:
                return oh5::IntConverter().convert(value);
            case Variant::STRING:
                return oh5::StringConverter().convert(value);
            case Variant::DATE:
                return oh5::DateConverter().convert(value);
            case Variant::TIME:
                return oh5::TimeConverter().convert(value);
            case Variant::BOOL:
                return oh5::BoolConverter().convert(value);
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


QString
TempH5File::path() const {
    return QString::fromUtf8(path_.get());
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
