#ifndef BRFC_DATA_OBJECT_H
#define BRFC_DATA_OBJECT_H

#include <brfc/assert.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/Attribute.hpp>

#include <deque>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace brfc {

class File;

template<typename T>
class DataObjectIterator;

/**
 * @brief a dataobject in HDF5 file
 *
 * bound with attribute and file
 * has (optional) parent and children
 */
class DataObject : public boost::noncopyable {
  public:
    typedef DataObjectIterator<DataObject> iterator;
    typedef DataObjectIterator<const DataObject> const_iterator;
    typedef std::vector<shared_ptr<Attribute> > AttributeVector;


    /**
     * @brief constructor
     * @param name name of this DataObject
     * @param file pointer to File containing this DataObject
     */
    explicit DataObject(const std::string& name, const File* file=0)
            : name_(name)
            , parent_(0)
            , children_()
            , attrs_()
            , file_(file) {

    }

    /**
     * @brief destructor
     */
    ~DataObject();
    
    /**
     * @brief is this DataObject the root of the hierarchy
     * @return true if this object has no parent
     */
    bool is_root() const {
        return parent_ == 0;
    }

    /**
     * @brief add a child Dataobject to this DataObject
     * @param name name of this DataObject
     *
     * DataObjects share common File
     */
    DataObject& add_child(const std::string& name);

    /**
     * @brief get hold of a child DataObject
     * @param name name of this child DataObject
     * @param create should the child be created if it's missing
     * @return reference to the child
     * @throw lookup_error if child does not exist and create is false
     */
    DataObject& child(const std::string& name, bool create=false);

    /**
     * @brief get full path of this DataObject
     * 
     * path is the concatenation of all names up to root node,
     * separated by '/'
     */
    std::string path() const;

    /**
     * @brief add an Attribute to this DataObject
     * @param name name of the Attribute
     * @param value Attribute value
     */
    void add_attribute(const std::string& name,
                       const QVariant& value);
    
    //@{
    /**
     * @brief get hold of an Attribute
     * @param name Attribute name
     * @return reference to requested Attribute
     * @throw lookup_error if no such Attribute is attached to this DataObject
     */
    const Attribute& attribute(const std::string& name) const;
    Attribute& attribute(const std::string& name);
    //@}


    /**
     * @brief check if this DataObject has an Attribute
     * @param name Attribute name
     * @return true if this DataObject has an Attribute identified by \c name
     */
    bool has_attribute(const std::string& name) const;
    
    /**
     * @brief get hold of parent DataObject
     */
    const DataObject* parent() const {
        return parent_;
    }
    
    /**
     * @brief name of this DataObject
     */
    const std::string& name() const {
        return name_;
    }

    /**
     * @brief get File owning this DataObject
     */
    const File* file() const {
        return file_;
    }

    /**
     * @brief vector of Attributes
     */
    const AttributeVector& attributes() const {
        return attrs_;
    }
    
    //@{
    /**
     * @brief iterator from this DataObject
     */
    iterator begin();
    const_iterator begin() const;
    //@}
    
    //@{
    /**
     * @brief iterator marking DataObject hierarchy end
     */
    iterator end();
    const_iterator end() const; 
    //@}

  private:
    friend class DataObjectIterator<const DataObject>;
    friend class DataObjectIterator<DataObject>;

    typedef std::vector<shared_ptr<DataObject> > ChildVector;

    DataObject(const std::string& name,
               const DataObject* parent);

    /**
     * @brief set the parent DataObject
     */
    void set_parent(DataObject* parent) {
        parent_ = parent;
    }


    std::string name_;
    const DataObject* parent_;
    ChildVector children_;
    AttributeVector attrs_;
    const File* file_;
};

/**
 * @brief iterator over DataObject hierarchy
 */
template<typename T>
class DataObjectIterator :
        public boost::iterator_facade<DataObjectIterator<T>,
                                      T,
                                      boost::forward_traversal_tag> {
  public:
    /**
     * @brief construct an empty iterator
     *
     * this also marks the end of the hierarchy
     */
    DataObjectIterator()
            : stack_() {
    }

    /**
     * @brief construct pointing to a DataObject
     * @param dobj DataObject to start iterating from
     */
    explicit DataObjectIterator(T* dobj)
            : stack_() {
        stack_.push_back(dobj);
    }

    /**
     * @brief copy constructor
     */
    template<typename OtherT>
    DataObjectIterator(const DataObjectIterator<OtherT>& other)
            : stack_(other.stack_.begin(), other.stack_.end()) {
    }

  private:
    friend class boost::iterator_core_access;
    template<typename>
    friend class DataObjectIterator;

    /**
     * @brief advance this iterator
     *
     * if there are elements in the stack, pop from the front
     * and add it's children to the back of the stack.
     */
    void increment() {
        if (not stack_.empty()) {
            T* cur = stack_.front();
            stack_.pop_front();
            BOOST_FOREACH(shared_ptr<T> child, cur->children_) {
                stack_.push_back(child.get());
            }
        }
    }

    /**
     * @brief value access
     */
    T& dereference() const {
        BRFC_ASSERT(not stack_.empty());
        return *stack_.front();
    }

    /**
     * @brief equality comparison
     *
     * two iterators are equal if their stacks are empty,
     * or if they have the same element in front of the stack.
     */
    template<typename OtherT>
    bool equal(const DataObjectIterator<OtherT>& rhs) const {
        if (stack_.empty() && rhs.stack_.empty()) {
            return true;
        } else if (not stack_.empty() && not rhs.stack_.empty()) {
            return stack_.front() == rhs.stack_.front();
        } else {
            return false;
        }
    }

    std::deque<T*> stack_;
};

}

#endif // BRFC_DATA_OBJECT_H
