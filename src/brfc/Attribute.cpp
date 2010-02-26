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
    if (data_object_ == 0) {
        return name_;
    } else {
        return data_object_->path() + "/" + name_;
    }
}

QString
Attribute::to_string() const {
    return path() + "=" + value_->to_string();
}

} // namespace brfc
