#ifndef BRFC_OH5_ROOT_HPP
#define BRFC_OH5_ROOT_HPP

#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief the root group
 *
 * can have no parent
 * can have Attribute, AttributeGroup or DataSet as child
 */
class Root : public Group {
  public:
    virtual ~Root();

    using Group::file;

    /**
     * @brief associate with a file
     */
    void file(shared_ptr<File> file) {
        file_ = file;
    }

  protected:
    template<class T>
    friend
    shared_ptr<T> boost::make_shared();

    /**
     * @brief constructor
     */
    Root();

    /**
     * @return false
     */
    virtual bool do_accepts_parent(const Node& node) const {
        return false;
    }

    /**
     * accepts Attribute, AttributeGroup and Dataset as children
     */
    virtual bool do_accepts_child(const Node& node) const;

    /**
     * @return pointer to File or null if not associated
     */
    virtual shared_ptr<const File> do_file() const {
        return file_.lock();
    }
  
  private:
    weak_ptr<File> file_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ROOT_HPP
