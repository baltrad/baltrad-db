#include "TempH5File.hpp"

#include <stdexcept>

#include <QtCore/QVariant>

#include <unistd.h>
#include <cstdlib>
#include <cstring>

namespace brfc {

TempH5File::TempH5File()
        : nodes_(HLNodeList_new(), &HLNodeList_free)
        , filename_(strdup("/tmp/brfctest_XXXXXX"), &free)
        , fd_(mkstemp(filename_.get())) {
    if (fd_ == -1)
        throw std::runtime_error("could not create temporary file");
    close(fd_);
}

TempH5File::~TempH5File() {
    unlink(filename());
}

auto_ptr<TempH5File>
TempH5File::minimal(const std::string& object,
                    const QDate& date,
                    const QTime& time,
                    const std::string& source,
                    const std::string& version) {
    auto_ptr<TempH5File> f(new TempH5File());
    f->add_attribute("/Conventions", "ODIM_H5/V2_0");
    f->add_group("/what");
    f->add_attribute("/what/object", QString::fromStdString(object));
    f->add_attribute("/what/version", QString::fromStdString(version));
    f->add_attribute("/what/date", date);
    f->add_attribute("/what/time", time);
    f->add_attribute("/what/source", QString::fromStdString(source));

    return f;
}

void TempH5File::add_group(const char* path) {
    std::string node_path(path);
    HL_Node* node = HLNode_newGroup(path);
    if (node == 0)
        throw std::runtime_error("could not create dataset: " + node_path);
    if (HLNodeList_addNode(nodes_.get(), node) == 0)
        throw std::runtime_error("could not add node: " + node_path);
}

void
TempH5File::add_attribute(const char* path, const QVariant& value) {
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
TempH5File::filename() const {
    return filename_.get();
}

void
TempH5File::write() {
    HL_Compression compression;
    HLCompression_init(&compression, CT_ZLIB);
    compression.level = 6;
    if (HLNodeList_setFileName(nodes_.get(), filename()) == 0)
        throw std::runtime_error("could not set filename");
    if (HLNodeList_write(nodes_.get(), 0, &compression) == 0)
        throw std::runtime_error("could not write file");
}

HL_Data TempH5File::convert(const QVariant& value) {
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
