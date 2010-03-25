#include <brfc/oh5/Quality.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Data.hpp>

namespace brfc {
namespace oh5 {

Quality::Quality(const QString& name)
        : Group(name) {
}

Quality::~Quality() {

}

bool
Quality::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const AttributeGroup*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
