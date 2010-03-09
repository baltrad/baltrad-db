#ifndef BRFC_OH5_NODE_HPP
#define BRFC_OH5_NODE_HPP

#include <deque>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <QtCore/QString>

#include <brfc/smart_ptr.hpp>

namespace brfc {
namespace oh5 {

class File;

template<typename T>
class NodeIterator;

class Node : public boost::noncopyable,
             public enable_shared_from_this<Node> {
  public:
    typedef std::vector<shared_ptr<Node> > ChildVector;
    typedef NodeIterator<Node> iterator;
    typedef NodeIterator<const Node> const_iterator;

    virtual ~Node();

    const QString& name() const { return name_; }

    void name(const QString& name);

    QString path() const;

    shared_ptr<Node> parent() const {
        return parent_.lock();
    }

    /**
     * @brief is this node the root node
     *
     */
    bool is_root() const {
        return parent_.expired();
    }
    
    /**
     * @{
     * @brief get root node
     */
    shared_ptr<Node> root();

    shared_ptr<const Node> root() const;
    ///@}

    /**
     * @brief add a child node
     * @throw duplicate_entry child with name exists
     * @throw value_error child not accepted
     *
     * @sa accepts_child
     */
    void add_child(shared_ptr<Node> child);

    bool has_child_by_name(const QString& name) const;

    bool has_child(const Node& node) const;

    void remove_child(Node& node);
    
    /**
     * @{
     */
    shared_ptr<Node>
    child_by_name(const QString& name);

    shared_ptr<const Node>
    child_by_name(const QString& name) const;
    ///@}
    
    /**
     * @{
     */
    shared_ptr<Node>
    child_by_path(const QString& path);

    shared_ptr<const Node>
    child_by_path(const QString& path) const;
    ///@}

    /**
     * @sa do_accepts_child
     */
    bool accepts_child(const Node& node) const {
        return do_accepts_child(node);
    }

    /**
     * @sa do_accepts_parent
     */
    bool accepts_parent(const Node& node) const {
        return do_accepts_parent(node);
    }

    const ChildVector& children() const {
        return children_;
    }

    /**
     * @brief file this node is associated with
     *
     * default implementation returns the file associated with root and
     * for root returns a null pointer.
     * @{
     */
    shared_ptr<const File> file() const {
        return do_file();
    }

    shared_ptr<File> file() {
        const Node* self = const_cast<const Node*>(this);
        return const_pointer_cast<File>(self->file());
    }
    ///@}
    
    iterator begin();
    const_iterator begin() const;
    
    iterator end();
    const_iterator end() const;

  protected:
    // allow make_shared access to constructor
    /*
     * could use this for gcc >= 4.3 with -std=c++x0
    template<typename T, typename... Args> 
    friend shared_ptr<T> boost::make_shared(Args&& ...); 
    */

    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);

    /**
     * @brief constructor
     *
     * use make_shared<Node> to call
     */
    Node(const QString& name);

    void parent(shared_ptr<Node> node);
    
    virtual bool do_accepts_child(const Node& node) const = 0;

    virtual bool do_accepts_parent(const Node& node) const = 0;

    virtual shared_ptr<const File> do_file() const;

  private:
    friend class NodeIterator<Node>;
    friend class NodeIterator<const Node>;

    QString name_;
    weak_ptr<Node> parent_;
    ChildVector children_;

};

template<typename T>
class NodeIterator :
        public boost::iterator_facade<NodeIterator<T>,
                                      T,
                                      boost::forward_traversal_tag> {
  public:
    NodeIterator();

    explicit NodeIterator(T& node);

    template<typename OtherT>
    NodeIterator(const NodeIterator<OtherT>& other);

  private:
    friend class boost::iterator_core_access;
    template<typename>
    friend class NodeIterator;
    
    void increment();

    T& dereference() const;

    template<typename OtherT>
    bool equal(const NodeIterator<OtherT>& rhs) const;

    std::deque<shared_ptr<T> > stack_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_NODE_HPP
