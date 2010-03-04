#include <brfc/oh5/Attribute.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Variant.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataObject.hpp>

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

shared_ptr<AttributeGroup>
Attribute::group() const {
    return dynamic_pointer_cast<AttributeGroup>(parent());
}

shared_ptr<DataObject>
Attribute::data_object() const {
    shared_ptr<Node> dobj;

    if (group()) {
        dobj = group()->parent();
    } else {
        dobj = parent();
    }
    return dynamic_pointer_cast<DataObject>(dobj);
}

void
Attribute::value(const Variant& value) {
    *value_ = value;
}

QString
Attribute::full_name() const {
    if (group()) {
        return group()->name() + "/" + name();
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
