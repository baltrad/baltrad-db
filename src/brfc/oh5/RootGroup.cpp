#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataSetGroup.hpp>

namespace brfc {
namespace oh5 {

RootGroup::~RootGroup() { }

RootGroup::RootGroup()
        : Group("")
        , file_() {

}

bool
RootGroup::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const Attribute*>(&node) != 0 or
        dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const DataSetGroup*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
