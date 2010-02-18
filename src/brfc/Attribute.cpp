#include <brfc/Attribute.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

Attribute::Attribute(const QString& name,
                     const Variant& value,
                     const DataObject* data_object)
        : data_object_(data_object)
        , name_(name)
        , value_(new Variant(value)) {
}

Attribute::~Attribute() {

}

void
Attribute::value(const Variant& value) {
    *value_ = value;
}

QString
Attribute::path() const {
    return data_object_->path() + QString::fromUtf8("/") + name_;
}

} // namespace brfc
