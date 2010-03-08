#ifndef BRFC_OH5_ATTRIBUTE_GROUP_HPP
#define BRFC_OH5_ATTRIBUTE_GROUP_HPP

#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief a Group that contains only Attributes
 */
class AttributeGroup : public Group {
  protected:
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     *
     * use make_shared<Node> to call
     */
    AttributeGroup(const QString& name)
        : Group(name) {
    }

    virtual bool do_accepts_child(const Node& node) const;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_GROUP_HPP
