/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    
    QString dataset_group() const { return dataset_group_; }
    QString data_group() const { return data_group_; }
    QString quality_group() const { return quality_group_; }
    QString attribute_group() const { return attribute_group_; }
    QString attribute() const { return attribute_; }
    QString dataset() const { return dataset_; }

  private:
    void split(const QString& path);

    QString dataset_group_;
    QString data_group_;
    QString quality_group_;
    QString attribute_group_;
    QString attribute_;
    QString dataset_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SPLIT_PATH_HPP
