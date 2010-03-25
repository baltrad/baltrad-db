#ifndef BRFC_OH5_DATA_SET_HPP
#define BRFC_OH5_DATA_SET_HPP

#include <brfc/oh5/Node.hpp>

namespace brfc {
namespace oh5 {

/**
 * ODIM_H5 dataset
 *
 * - can have Attribute as a child
 */
class DataSet : public Node {
  public:
    virtual ~DataSet();

  protected:
    template<class T, class A1>
    friend
    shared_ptr<T> boost::make_shared(const A1&);

    explicit DataSet(const QString& name="data");

    virtual bool do_accepts_child(const Node& node) const;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_DATA_SET_HPP
