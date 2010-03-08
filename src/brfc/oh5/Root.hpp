#ifndef BRFC_OH5_ROOT_HPP
#define BRFC_OH5_ROOT_HPP

namespace brfc {
namespace oh5 {

/**
 * @brief the root group
 *
 * can have no parent
 */
class Root : public Group {
  protected:
    template<class T>
    friend
    shared_ptr<T> boost::make_shared();

    /**
     * @brief constructor
     */
    Root()
            : Group("") {
    }

    /**
     * @return false
     */
    virtual bool do_accepts_parent(const Node& node) const {
        return false;
    }
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_ROOT_HPP
