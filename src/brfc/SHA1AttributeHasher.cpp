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

#include <boost/foreach.hpp>

#include <QtCore/QByteArray>
#include <QtCore/QCryptographicHash>
#include <QtCore/QStringList>

#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/Source.hpp>
#include <brfc/oh5/RootGroup.hpp>

namespace brfc {

SHA1AttributeHasher::SHA1AttributeHasher(
    shared_ptr<const oh5::AttributeSpecs> specs)
        : specs_(specs) {
}

SHA1AttributeHasher::~SHA1AttributeHasher() {

}

QString
SHA1AttributeHasher::do_name() const {
    return "sha1_attribute";
}

QString
SHA1AttributeHasher::do_hash(const oh5::File& file) {
    if (not file.source()) {
        //XXX: needs a better exception type
        throw value_error("can't form unique_id: not associated with source");
    }
    
    QStringList strs(file.source()->node_id());

    const oh5::Attribute* attr = 0;
    const oh5::AttributeSpec* spec = 0;
    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        attr = dynamic_cast<const oh5::Attribute*>(&node);
        if (not attr or not attr->is_valid())
            continue;

        // this might throw, but we should have all the specs
        spec = &specs_->get(attr->full_name());
        if (not spec->ignore_in_hash) {
            strs.append(attr->to_string());
        }
    }
    strs.sort(); // ensure same order

    QByteArray bytes = strs.join("").toUtf8();
    QByteArray hash = QCryptographicHash::hash(bytes,
                                               QCryptographicHash::Sha1);
    return QString::fromAscii(hash.toHex());
}

} // namespace brfc
