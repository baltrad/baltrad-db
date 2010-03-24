#ifndef BRFC_OH5_GROUP_HPP
#define BRFC_OH5_GROUP_HPP

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

class Attribute;

/**
 * @brief ABC for ODIM_H5 elements that can have children
 */
class Group : public Node {
  public:
    /**
     * @brief destructor
     */
    virtual ~Group();

    /**
     * @{
     * @return pointer to Attribute or null if not found
     */
    shared_ptr<Attribute> attribute(const QString& name);

    shared_ptr<const Attribute> attribute(const QString& name) const;
    ///@}
    
    /**
     * @{
     * @brief access child group by name
     * @return pointer to Group or null if not found
     */
    shared_ptr<Group> child_group_by_name(const QString& name);

    shared_ptr<const Group> child_group_by_name(const QString& name) const;
    ///@}

    // XXX: this needs to go!
    long long db_id() const { return db_id_; }

    void db_id(long long db_id) const {
        db_id_ = db_id;
    }

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

    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual bool do_accepts_parent(const Node& node) const;
  
  private:
    mutable long long db_id_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
