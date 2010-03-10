#include "TempH5File.hpp"

#include <brfc/oh5/Converter.hpp>
#include <brfc/Variant.hpp>

#include <stdexcept>

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
    unlink(filename_.get());
}

auto_ptr<TempH5File>
TempH5File::minimal(const QString& object,
                    const QDate& date,
                    const QTime& time,
                    const QString& source,
                    const QString& version) {
    auto_ptr<TempH5File> f(new TempH5File());
    f->add_attribute("/Conventions", Variant("ODIM_H5/V2_0"));
    f->add_group("/what");
    f->add_attribute("/what/object", Variant(object));
    f->add_attribute("/what/version", Variant(version));
    f->add_attribute("/what/date", Variant(date));
    f->add_attribute("/what/time", Variant(time));
    f->add_attribute("/what/source", Variant(source));

    return f;
}

void TempH5File::add_group(const char* path) {
    HL_Node* node = HLNode_newGroup(path);
    if (node == 0)
        throw std::runtime_error("could not create dataset");
    if (HLNodeList_addNode(nodes_.get(), node) == 0)
        throw std::runtime_error("could not add node");
}

namespace {

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

} // namespace anonymous


void
TempH5File::add_attribute(const char* path, const Variant& value) {
    // seek and create nodes
    HL_Node* node = HLNode_newAttribute(path);
    if (HLNodeList_addNode(nodes_.get(), node) == 0) {
        throw std::runtime_error("couldn't add node");
    }
    // convert value
    oh5::HL_Data d = convert(value);
    // add node
    HLNode_setScalarValue(node, d.size(), d.data(), d.type(), -1);
}

QString
TempH5File::filename() const {
    return QString::fromUtf8(filename_.get());
}

void
TempH5File::write() {
    HL_Compression compression;
    HLCompression_init(&compression, CT_ZLIB);
    compression.level = 6;
    if (HLNodeList_setFileName(nodes_.get(), filename_.get()) == 0)
        throw std::runtime_error("could not set filename");
    if (HLNodeList_write(nodes_.get(), 0, &compression) == 0)
        throw std::runtime_error("could not write file");
}

} // namespace brfc
