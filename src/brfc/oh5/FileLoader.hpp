#ifndef BRFC_OH5_FILE_LOADER_HPP
#define BRFC_OH5_FILE_LOADER_HPP

#include <QtCore/QStringList>

#include <brfc/smart_ptr.hpp>
#include <brfc/oh5/hlhdf.hpp>

namespace brfc {
namespace oh5 {

class AttributeSpecs;
class File;
class Group;
class SplitPath;

class FileLoader {
  public:
    FileLoader(const AttributeSpecs* specs);

    shared_ptr<File> load(const QString& path);

    ~FileLoader();
  
  private:
    shared_ptr<Group> get_or_create_group(const SplitPath& path);

    void add_attribute_from_node(HL_Node* node);

    const AttributeSpecs* specs_;
    shared_ptr<File> file_;
    QStringList attributegroup_names_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_FILE_LOADER_HPP
