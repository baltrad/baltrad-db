#include <brfc/oh5/DataSet.hpp>

#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/Data.hpp>
#include <brfc/oh5/Quality.hpp>

namespace brfc {
namespace oh5 {

DataSet::DataSet(const QString& name)
        : Group(name) {
}

DataSet::~DataSet() {

}

bool
DataSet::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const Data*>(&node) != 0 or
        dynamic_cast<const AttributeGroup*>(&node) != 0 or
        dynamic_cast<const Quality*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
