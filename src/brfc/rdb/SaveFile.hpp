#ifndef BRFC_RDB_SAVE_FILE_HPP
#define BRFC_RDB_SAVE_FILE_HPP

#include <brfc/visit.hpp>

#include <brfc/rdb/GroupIdCache.hpp>
#include <brfc/rdb/SaveAttribute.hpp>
#include <brfc/rdb/SaveGroup.hpp>

namespace brfc {

namespace oh5 {

class File;
class AttributeGroup;
class Group;
class Attribute;

}

namespace rdb {

/**
 * @brief save oh5::File instances to database
 */
class SaveFile {
  public:
    /**
     * @brief types accepted for visit()
     */
    typedef mpl::vector<const oh5::AttributeGroup,
                        const oh5::Group,
                        const oh5::Attribute> accepted_types;
    
    /**
     * @brief constructor
     * @param rdb database to save to
     */
    SaveFile(RelationalDatabase* rdb);
    
    /**
     * @brief save a oh5::AttributeGroup instance to database
     * 
     * this method does nothing, the group is contained in the
     * full name of the attribute.
     */
    void operator()(const oh5::AttributeGroup& group) {
        // no-op
    }
    
    /**
     * @brief save a oh5::Group instance to database
     *
     * save a group other than a oh5::AttributeGroup using a SaveGroup
     * functor.
     */
    void operator()(const oh5::Group& group);    
    
    /**
     * @brief save a oh5::Attribute instance to database
     *
     * instance is saved using a SaveAttribute functor.
     */
    void operator()(const oh5::Attribute& attribute);
    
    /**
     * @brief save a oh5::File instance to database
     * @param file the file to be saved
     * @param proposed_filename filename proposed for this filename
     * @param filename_version version number of the filename
     * @return database id of the saved file
     */
    long long operator()(const oh5::File& file,
                         const QString& proposed_filename,
                         unsigned int filename_version);

  private:
    RelationalDatabase* rdb_;
    GroupIdCache group_id_cache_;
    SaveAttribute save_attribute_;
    SaveGroup save_group_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RDB_SAVE_FILE_HPP
