#include <brfc/oh5/Group.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

Group::~Group() {

}

shared_ptr<Attribute>
Group::child_attribute(const QString& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->child_attribute(name));
}

shared_ptr<const Attribute>
Group::child_attribute(const QString& name) const {
    return dynamic_pointer_cast<const Attribute>(child_by_path(name));
}

shared_ptr<Attribute>
Group::attribute(const QString& name) {
    const Group* self = const_cast<const Group*>(this);
    return const_pointer_cast<Attribute>(self->attribute(name));
}

shared_ptr<const Attribute>
Group::attribute(const QString& name) const {
    return shared_ptr<const Attribute>();
}

shared_ptr<Group>
Group::child_group_by_name(const QString& name) {
    return dynamic_pointer_cast<Group>(child_by_name(name));
}

shared_ptr<const Group>
Group::child_group_by_name(const QString& name) const {
    return dynamic_pointer_cast<const Group>(child_by_name(name));
}

bool
Group::do_accepts_parent(const Node& node) const {
    if (dynamic_cast<const Group*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
