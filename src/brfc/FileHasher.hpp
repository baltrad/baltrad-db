/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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

#ifndef BRFC_FILE_HASHER_HPP
#define BRFC_FILE_HASHER_HPP

#include <list>
#include <string>

namespace brfc {

namespace oh5 {

class Oh5File;

} // namespace oh5

/**
 * @brief ABC for hashing a oh5::File
 *
 * generated hash should uniquely identify the file
 */
class FileHasher {
  public:
    /**
     * @brief constructor
     */
    explicit FileHasher(const std::string& name)
            : name_(name)
            , ignored_() {
    }

    virtual ~FileHasher() { }

    /**
     * @brief name of this hasher
     */
    std::string name() const {
        return name_;
    }

    /**
     * @brief hash a file
     */
    std::string hash(const oh5::Oh5File& file) const {
        return do_hash(file);
    }

    void ignore(const std::string& attribute) {
        ignored_.push_back(attribute);
    }

    const std::list<std::string>& ignored() const {
        return ignored_;
    }

    void clear_ignored() {
        ignored_.clear();
    }


  protected:
    virtual std::string do_hash(const oh5::Oh5File& file) const = 0;
  
  private:
    std::string name_;
    std::list<std::string> ignored_;
};

} // namespace brfc

#endif // BRFC_FILE_HASHER_HPP
