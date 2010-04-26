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

class SaveFile {
  public:
    typedef mpl::vector<const oh5::AttributeGroup,
                        const oh5::Group,
                        const oh5::Attribute> accepted_types;

    SaveFile(RelationalDatabase* rdb);

    void operator()(const oh5::AttributeGroup& group) {
        // no-op
    }

    void operator()(const oh5::Group& group);    

    void operator()(const oh5::Attribute& attribute);

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
