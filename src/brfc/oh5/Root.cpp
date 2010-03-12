#include <brfc/oh5/Root.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataSet.hpp>

namespace brfc {
namespace oh5 {

Root::~Root() { }

Root::Root()
        : Group("")
        , file_() {

}

bool
Root::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const Attribute*>(&node) != 0 or
        dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const DataSet*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
