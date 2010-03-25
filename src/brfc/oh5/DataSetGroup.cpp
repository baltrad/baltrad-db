#include <brfc/oh5/DataSetGroup.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/DataGroup.hpp>
#include <brfc/oh5/QualityGroup.hpp>

namespace brfc {
namespace oh5 {

DataSetGroup::DataSetGroup(const QString& name)
        : Group(name) {
}

DataSetGroup::~DataSetGroup() {

}

bool
DataSetGroup::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const DataGroup*>(&node) != 0 or
        dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const QualityGroup*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
