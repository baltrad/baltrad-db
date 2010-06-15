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

#include <brfc/rdb/SHA1AttributeHasher.hpp>

#include <boost/foreach.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/SHA1.hpp>
#include <brfc/StringList.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/rdb/AttributeSpecs.hpp>

namespace brfc {
namespace rdb {

SHA1AttributeHasher::SHA1AttributeHasher(
    shared_ptr<const AttributeSpecs> specs)
        : specs_(specs) {
}

SHA1AttributeHasher::~SHA1AttributeHasher() {

}

String
SHA1AttributeHasher::attribute_string(const oh5::Attribute& attr) {
    const Variant& value = attr.value();
    String value_str;
    switch (value.type()) {
        case Variant::DATE:
            value_str = value.date().to_string("yyyyMMdd");
            break;
        case Variant::TIME:
            value_str = value.time().to_string("hhmmss");
            break;
        default:
            value_str = value.to_string();
            break;
    }
    return attr.path() + "=" + value_str;
}

String
SHA1AttributeHasher::do_name() const {
    return "sha1_attribute";
}

String
SHA1AttributeHasher::do_hash(const oh5::File& file) {
    if (not file.source()) {
        //XXX: needs a better exception type
        throw value_error("can't form unique_id: not associated with source");
    }
    
    StringList strs(file.source()->node_id());

    const oh5::Attribute* attr = 0;
    const AttributeSpec* spec = 0;
    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        attr = dynamic_cast<const oh5::Attribute*>(&node);
        if (not attr or not attr->is_valid())
            continue;

        // this might throw, but we should have all the specs
        spec = &specs_->get(attr->full_name());
        if (not spec->ignore_in_hash) {
            strs.append(attribute_string(*attr));
        }
    }
    strs.sort(); // ensure same order
    
    CSHA1 sha1;

    std::string utf8 = strs.join("").to_utf8();
    std::string hash;

    sha1.Update((unsigned char*)utf8.c_str(), utf8.length());
    sha1.Final();
    sha1.ReportHashStl(hash, CSHA1::REPORT_HEX_SHORT);

    return String(hash).to_lower();
}

} // namespace rdb
} // namespace brfc
