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

shared_ptr<const Group>
Attribute::parent_group() const {
    shared_ptr<const AttributeGroup> parent_p =
        dynamic_pointer_cast<const AttributeGroup>(parent());
    if (parent_p)
        return dynamic_pointer_cast<const Group>(parent_p->parent());
    else
        return dynamic_pointer_cast<const Group>(parent());
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
