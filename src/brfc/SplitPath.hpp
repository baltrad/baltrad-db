#ifndef BRFC_SPLIT_PATH_HPP
#define BRFC_SPLIT_PATH_HPP

#include <QtCore/QString>

namespace brfc {

/**
 * @brief split a path to an Attribute in HDF5 file
 *
 * path will be split to three components:
 *  \li attribute name (prefixed with 'where/', 'what/' or 'how/' group if
 *                     applicable
 *  \li path to data object
 */
struct SplitPath {
    explicit SplitPath(const QString& path);

    QString attribute_name;
    QString data_object_path;
  
  private:
    void split(const QString& path);
};

}

#endif // BRFC_SPLIT_PATH_HPP
