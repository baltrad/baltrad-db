#include <brfc/oh5/DataSet.hpp>

#include <brfc/oh5/Attribute.hpp>

namespace brfc {
namespace oh5 {

DataSet::DataSet(const QString& name)
        : Node(name) { 

}

DataSet::~DataSet() {

}

bool
DataSet::do_accepts_child(const Node& node) const {
    if (dynamic_cast<const Attribute*>(&node) != 0) {
        return true;
    } else {
        return false;
    }
}

} // namespace oh5
} // namespace brfc
