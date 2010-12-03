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

#include <brfc/SHA1AttributeHasher.hpp>

#include <iomanip>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <brfc/exceptions.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {

SHA1AttributeHasher::SHA1AttributeHasher()
        : FileHasher("sha1_attribute") {

}

SHA1AttributeHasher::~SHA1AttributeHasher() {

}

std::string
SHA1AttributeHasher::attribute_string(const oh5::Attribute& attr) {
    return attr.path() + "=" + attr.value().to_string();
}

std::string
SHA1AttributeHasher::sha1hash(const std::string& str) {
    boost::uuids::detail::sha1 sha;
    sha.reset();
    sha.process_bytes(str.c_str(), str.size());

    unsigned int digest[5];
    sha.get_digest(digest);

    std::stringstream ss;
    for (int i = 0; i < 5; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(8) << std::right << digest[i];
    }
    
    return ss.str();
}

std::string
SHA1AttributeHasher::do_hash(const oh5::File& file) const {
    StringList strs;
    const oh5::Attribute* attr = 0;

    BOOST_FOREACH(const oh5::Node& node, file.root()) {
        attr = dynamic_cast<const oh5::Attribute*>(&node);
        if (attr and not ignored().contains(attr->full_name())) {
            strs.append(attribute_string(*attr));
        }
    }

    strs.sort(); // ensure same order
    
    std::string hash = sha1hash(strs.join(""));

    boost::to_lower(hash);
    return hash;
}

} // namespace brfc
