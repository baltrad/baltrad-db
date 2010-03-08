#include <brfc/oh5/Attribute.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

Attribute::Attribute(const QString& name,
                     const Variant& value,
                     bool ignore_in_hash)
        : Node(name)
        , value_(new Variant(value))
        , ignore_in_hash_(ignore_in_hash) {
}

Attribute::~Attribute() {

}

void
Attribute::value(const Variant& value) {
    *value_ = value;
}

QString
Attribute::full_name() const {
    shared_ptr<AttributeGroup> grp =
        dynamic_pointer_cast<AttributeGroup>(parent());
    if (grp) {
        return grp->name() + "/" + name();
    } else {
        return name();
    }
}

QString
Attribute::to_string() const {
    return path() + "=" + value_->to_string();
}

} // namespace oh5
} // namespace brfc
