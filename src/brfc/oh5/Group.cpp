#include <brfc/oh5/Group.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

shared_ptr<Attribute>
Group::attribute(const QString& name) {
    return dynamic_pointer_cast<Attribute>(child_by_path(name));
}

bool
Group::do_accepts_child(const Node& node) const {
    return true;
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
