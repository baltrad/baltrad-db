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

/**
 * @brief split a path to an Attribute in HDF5 file
 *
 * path will be split to three components:
 *  \li attribute name (prefixed with 'where/', 'what/' or 'how/' group if
 *                     applicable
 *  \li path to data object
 */
struct SplitPath {
    explicit SplitPath(const QString& path,
                       const QStringList& attributegroup_names);

    QString attribute_name;
    QString full_attribute_name;
    QString group_path;
  
  private:
    void split(const QString& path,
               const QStringList& attributegroup_names);
};


class FileLoader {
  public:
    FileLoader(const AttributeSpecs* specs);

    shared_ptr<File> load(const QString& path);

    ~FileLoader();
  
  private:
    shared_ptr<Group> get_or_create_group(const QString& path);

    void add_attribute_from_node(HL_Node* node);

    shared_ptr<Group> create_group(const QString& name);


    const AttributeSpecs* specs_;
    shared_ptr<File> file_;
    QStringList attributegroup_names_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_FILE_LOADER_HPP
