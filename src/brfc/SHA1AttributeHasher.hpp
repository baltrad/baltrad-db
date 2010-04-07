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

#ifndef BRFC_SHA1_ATTRIBUTE_HASHER_HPP
#define BRFC_SHA1_ATTRIBUTE_HASHER_HPP

#include <brfc/FileHasher.hpp>

namespace brfc {

namespace oh5 {

class AttributeSpecs;

} // namespace oh5

/**
 * @brief generate SHA-1 hash of ODIM_H5 metadata attributes
 */
class SHA1AttributeHasher : public FileHasher {
  public:
    explicit SHA1AttributeHasher(const oh5::AttributeSpecs* specs);

    virtual ~SHA1AttributeHasher();
  
  protected:
    /**
     * @return "sha1_attribute"
     */
    virtual QString do_name() const;

    /**
     * @brief generate the hash
     *
     * SHA-1 hash of concatenated string representation of all attributes
     * not marked to be ignored in hashing plus node_id of the associated
     * Source.
     *
     * @note changes to this algorithm most probably trigger the need to
     *       re-evaluate all files in the database hashed using this hasher.
     */
    virtual QString do_hash(const oh5::File& file);
  
  private:
    const oh5::AttributeSpecs* specs_;
};

} // namespace brfc

#endif // BRFC_SHA1_ATTRIBUTE_HASHER_HPP
