#ifndef BRFC_OH5_DATA_SET_GROUP_HPP
#define BRFC_OH5_DATA_SET_GROUP_HPP

#include <brfc/oh5/Group.hpp>

namespace brfc {
namespace oh5 {

/**
 * @brief ODIM_H5 dataset group
 *
 * can have Data or AttributeGroup or Quality as a child
 */
class DataSetGroup : public Group {
  public:
    virtual ~DataSetGroup();

  protected:
    // allow make_shared access to constructor
    template<class T, class A1> 
    friend 
    shared_ptr<T> boost::make_shared(const A1& a1);
    

    /**
     * @brief constructor
     */
    explicit DataSetGroup(const QString& name);
    
    /**
     * @return true if node is Data or AttributeGroup
     */
    virtual bool do_accepts_child(const Node& node) const;

};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_DATA_SET_GROUP_HPP
