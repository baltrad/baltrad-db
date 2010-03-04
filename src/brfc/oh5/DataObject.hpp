#ifndef BRFC_DATA_OBJECT_HPP
#define BRFC_DATA_OBJECT_HPP

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

class DataObject : public Node {
  protected:
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     *
     * use make_shared<Node> to call
     */
    DataObject(const QString& name)
        : Node(name) {
    }

    virtual bool do_accepts_child(const Node& node) const;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_DATA_OBJECT_HPP
