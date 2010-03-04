#ifndef BRFC_NODE_HPP
#define BRFC_NODE_HPP

#include <vector>

#include <boost/noncopyable.hpp>
#include <QtCore/QString>

#include <brfc/smart_ptr.hpp>

namespace brfc {
namespace oh5 {

class Node : public boost::noncopyable,
             public enable_shared_from_this<Node> {
  public:
    typedef std::vector<shared_ptr<Node> > ChildVector;

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
     * @sa do_accepts_child
     */
    bool accepts_child(const Node& node) const {
        return do_accepts_child(node);
    }

    const ChildVector& children() const {
        return children_;
    }

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
    
    /**
     * default implementation always returns true
     */
    virtual bool do_accepts_child(const Node& child) const {
        return true;
    }

  private:
    QString name_;
    weak_ptr<Node> parent_;
    ChildVector children_;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_NODE_HPP
