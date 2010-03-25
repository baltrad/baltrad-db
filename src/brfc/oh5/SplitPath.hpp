#ifndef BRFC_OH5_SPLIT_PATH_HPP
#define BRFC_OH5_SPLIT_PATH_HPP

#include <QtCore/QString>

class QStringList;

namespace brfc {
namespace oh5 {

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
    
    /**
     * @name DEPRECATED
     * @{
     */
    QString attribute_name() const { return attribute_; }
    QString full_attribute_name() const;
    QString group_path() const;
    ///@}
    
    QString dataset() const { return dataset_; }
    QString data() const { return data_; }
    QString quality() const { return quality_; }
    QString attribute_group() const { return attribute_group_; }
    QString attribute() const { return attribute_; }

  private:
    void split(const QString& path);

    QString dataset_;
    QString data_;
    QString quality_;
    QString attribute_group_;
    QString attribute_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SPLIT_PATH_HPP
