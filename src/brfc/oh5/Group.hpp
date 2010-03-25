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
     * @brief access a child attribute
     * @param name name of the attribute, can contain a group (e.g what/date)
     * @return pointer to Attribute or null if not found
     * 
     * The search for the attribute is performed among immediate children.
     * If the name is prefixed with a group, the search is performed in
     * the respective AttributeGroup.
     */
    shared_ptr<Attribute> child_attribute(const QString& name);

    shared_ptr<const Attribute> child_attribute(const QString& name) const;
    ///@}
    
    /**
     * @{
     * @brief access a child attribute "in effect"
     * @param name name of the attribute, can contain a group (e.g what/date)
     * @return pointer to Attribute or null if not found
     *
     * This method seeks for an attribute "in effect" at this hierarchy
     * level. The search is performed among immediate children (if prefixed
     * with a group name, also in respective group). If no match is found at
     * current level, the search is continued up the hierarchy until a match
     * is found or root node is reached.
     *
     * So given a hierarchy:
     *  - /
     *    - what
     *      - attr1
     *      - attr2
     *    - dataset1
     *      - attr1
     *      - data1
     *        - what
     *          - attr1
     *          - attr2
     *      - data2
     * 
     * following results would be obtained:
     * 
     * - search for attr1 @ data2 would return /dataset1/attr1
     * - search for attr2 @ data2 would return null
     * - search for what/attr1 @ data2 would return /what/attr1
     * - search for what/attr1 @ data1 would return /dataset1/data1/what/attr1
     * - and so on ...
     * 
     */
    shared_ptr<Attribute> attribute(const QString& name);

    shared_ptr<const Attribute> attribute(const QString& name) const;
    ///@}
    
    /**
     * @{
     * @brief access child group by name
     * @param name name of the group
     * @return pointer to Group or null if not found
     *
     * The search for the group is performed among immediate children.
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
        : Node(name)
        , db_id_(0) {
    }

    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual bool do_accepts_parent(const Node& node) const;
  
  private:
    mutable long long db_id_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_GROUP_HPP
