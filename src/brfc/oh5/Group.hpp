#ifndef BRFC_OH5_GROUP_HPP
#define BRFC_OH5_GROUP_HPP

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

class Attribute;

class Group : public Node {
  public:
    /**
     * @throw lookup_error when attribute can't be found
     */
    shared_ptr<Attribute> attribute(const QString& path);

  protected:
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     */
    Group(const QString& name)
        : Node(name) {
    }

    virtual bool do_accepts_child(const Node& node) const;

    virtual bool do_accepts_parent(const Node& node) const;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
