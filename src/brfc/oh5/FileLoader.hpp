#ifndef BRFC_OH5_FILE_LOADER_HPP
#define BRFC_OH5_FILE_LOADER_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/oh5/hlhdf.hpp>

class QString;

namespace brfc {
namespace oh5 {

class AttributeSpecs;
class File;
class Group;
class SplitPath;

/**
 * @brief load metadata from a file in filesystem
 */
class FileLoader {
  public:
    /**
     * @brief constructor
     * @param specs used for converter lookups
     */
    explicit FileLoader(const AttributeSpecs* specs);

    /**
     * @brief desctructor
     */
    ~FileLoader();
  
    /**
     * @brief load from filesystem
     * @param path path in filesystem
     * @return File pointer
     * @throw fs_error if the file can not be opened
     *
     * ignored attributes (failing name or converter lookup) are stored
     * as full attribute paths to ignored_attributes on File
     */
    shared_ptr<File> load(const QString& path);

  private:
    shared_ptr<Group> get_or_create_group(const SplitPath& path);

    void add_attribute_from_node(HL_Node* node);

    const AttributeSpecs* specs_;
    shared_ptr<File> file_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_FILE_LOADER_HPP
