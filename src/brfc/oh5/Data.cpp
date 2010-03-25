#include <brfc/oh5/Data.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Quality.hpp>

namespace brfc {
namespace oh5 {

Data::Data(const QString& name)
        : Group(name) {
}

Data::~Data() {

}

bool
Data::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const Quality*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
