#include <brfc/Attribute.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Variant.hpp>

namespace brfc {

Attribute::Attribute(const std::string& name,
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

std::string
Attribute::path() const {
    return data_object_->path() + "/" + name_;
}

} // namespace brfc
