#include <brfc/oh5/DataGroup.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/QualityGroup.hpp>

namespace brfc {
namespace oh5 {

DataGroup::DataGroup(const QString& name)
        : Group(name) {
}

DataGroup::~DataGroup() {

}

bool
DataGroup::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const QualityGroup*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
