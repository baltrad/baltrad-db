#include <brfc/oh5/QualityGroup.hpp>

#include <brfc/oh5/AttributeGroup.hpp>

namespace brfc {
namespace oh5 {

QualityGroup::QualityGroup(const QString& name)
        : Group(name) {
}

QualityGroup::~QualityGroup() {

}

bool
QualityGroup::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const AttributeGroup*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
