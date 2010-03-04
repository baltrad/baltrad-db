#include <brfc/oh5/DataObject.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

bool
DataObject::do_accepts_child(const Node& node) const {
    return true;
}

} // namespace oh5
} // namespace brfc
