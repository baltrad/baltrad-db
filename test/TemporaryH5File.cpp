#include "TemporaryH5File.hpp"

#include <stdexcept>

#include <QtCore/QVariant>

#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>

namespace brfc {

TemporaryH5File::TemporaryH5File()
        : nodes_(HLNodeList_new(), &HLNodeList_free)
        , filename_(strdup("/tmp/brfctest_XXXXXX"), &free)
        , fd_(mkstemp(filename_.get())) {
    if (fd_ == -1)
        throw std::runtime_error("could not create temporary file");
    close(fd_);
}

TemporaryH5File::~TemporaryH5File() {
    unlink(filename());
}

void TemporaryH5File::add_group(const char* path) {
    std::string node_path(path);
    HL_Node* node = HLNode_newGroup(path);
    if (node == 0)
        throw std::runtime_error("could not create dataset: " + node_path);
    if (HLNodeList_addNode(nodes_.get(), node) == 0)
        throw std::runtime_error("could not add node: " + node_path);
}

void
TemporaryH5File::add_attribute(const char* path, const QVariant& value) {
    // seek and create nodes
    HL_Node* node = HLNode_newAttribute(path);
    if (HLNodeList_addNode(nodes_.get(), node) == 0) {
        throw std::runtime_error("couldn't add node");
    }
    // convert value
    HL_Data d = convert(value);
    // add node
    HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
}

const char*
TemporaryH5File::filename() const {
    return filename_.get();
}

void
TemporaryH5File::write() {
    HL_Compression compression;
    HLCompression_init(&compression, CT_ZLIB);
    compression.level = 6;
    if (HLNodeList_setFileName(nodes_.get(), filename()) == 0)
        throw std::runtime_error("could not set filename");
    if (HLNodeList_write(nodes_.get(), 0, &compression) == 0)
        throw std::runtime_error("could not write file");
}

HL_Data TemporaryH5File::convert(const QVariant& value) {
    switch (value.type()) {
        case QVariant::Double:
            return RealConverter().convert(value);
        case QVariant::LongLong:
        case QVariant::Int:
            return IntConverter().convert(value);
        case QVariant::String:
            return StringConverter().convert(value);
        case QVariant::Date:
            return DateConverter().convert(value);
        case QVariant::Time:
            return TimeConverter().convert(value);
        default:
            throw std::runtime_error("could not convert");
    }
}

} // namespace brfc
