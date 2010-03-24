#ifndef BRFC_OH5_ATTRIBUTE_H
#define BRFC_OH5_ATTRIBUTE_H

#include <brfc/Variant.hpp>
#include <brfc/oh5/Node.hpp>

namespace brfc {

namespace oh5 {

class Group;

/**
 * @brief ODIM_H5 attribute element
 */
class Attribute : public Node {
  public:    
    /**
     * @brief destructor
     */
    virtual ~Attribute();
    
    /**
     * @brief return nearest parent Group that is not AttributeGroup
     * @return Group or null if not found
     *
     * if the attribute is in an AttributeGroup, bypass it
     *
     * @{
     */
    shared_ptr<const Group> parent_group() const;

    shared_ptr<Group> parent_group();
    /// @}
    
    /**
     * @brief attribute value
     */
    const Variant& value() const { return value_; }

    /**
     * @brief set attribute value
     */
    void value(const Variant& value);

    /**
     * @brief is the attribute valid
     *
     * attribute is considered valid if it has a value (Variant is not null)
     */
    bool is_valid() const;

    bool ignore_in_hash() const { return ignore_in_hash_; }

    void ignore_in_hash(bool ignore_in_hash) {
        ignore_in_hash_ = ignore_in_hash;
    }

    /**
     * @brief full name of this attribute
     *
     * if the attribute is in an AttributeGroup, return the name with
     * group name prepended.
     */
    QString full_name() const;
    
    /**
     * @brief string representation of this attribute
     *
     * /path/to/attribute=value
     */
    QString to_string() const;

  protected:
    template<class T, class A1>
    friend
    shared_ptr<T> boost::make_shared(const A1&);

    template<class T, class A1, class A2> 
    friend 
    shared_ptr<T> boost::make_shared(const A1&, const A2&);

    template<class T, class A1, class A2, class A3> 
    friend 
    shared_ptr<T> boost::make_shared(const A1&, const A2&, const A3&);

    /**
     * @brief constructor
     * @param name name of the attribute
     * @param value attribute value
     * @param ignore_in_hash should this attribute be ignored when hashing
     */
    explicit Attribute(const QString& name,
                       const Variant& value=Variant(),
                       bool ignore_in_hash=false);

    /**
     * @return false
     *
     * Attribute is last in the hierarchy and doesn't accept any children 
     */
    virtual bool do_accepts_child(const Node& node) const {
        return false;
    }

    /**
     * @return true
     *
     * relies on the parent node to deny it
     */
    virtual bool do_accepts_parent(const Node& node) const {
        return true;
    }

  private:
    Variant value_;
    bool ignore_in_hash_;
};


} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ATTRIBUTE_H
