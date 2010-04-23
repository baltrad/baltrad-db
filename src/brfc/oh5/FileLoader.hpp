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
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

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
     * attributes for which name and/or converter lookup fails, are stored
     * with an empty Variant as a value (meaning they are invalid). 
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
