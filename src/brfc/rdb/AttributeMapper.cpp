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

#include <brfc/rdb/AttributeMapper.hpp>

#include <brfc/exceptions.hpp>

#include <boost/foreach.hpp>

namespace brfc {
namespace rdb {

AttributeMapper::AttributeMapper()
        : mappings_() {
}

AttributeMapper::AttributeMapper(const AttributeMapper& other)
        : mappings_(other.mappings_) {

}

AttributeMapper&
AttributeMapper::operator=(const AttributeMapper& rhs) {
    if (this != &rhs) {
        mappings_ = rhs.mappings_;
    }
    return *this;
}

AttributeMapper::~AttributeMapper() {

}

void
AttributeMapper::add(const Mapping& mapping) {
    if (has(mapping.attribute))
        throw duplicate_entry(mapping.attribute.toUtf8().constData());
    mappings_.insert(std::make_pair(mapping.attribute, mapping));
}

bool
AttributeMapper::has(const QString& attribute) const {
    return mappings_.find(attribute) != mappings_.end();
}

bool
AttributeMapper::is_specialized(const QString& attribute) const {
    QString s = QString::fromUtf8("attribute_values");
    return not mapping(attribute).table.startsWith(s);
}

MappingVector
AttributeMapper::specializations_on(const QString& table) const {
    MappingVector vec;
    BOOST_FOREACH(const MappingMap::value_type& entry, mappings_) {
        const Mapping& mapping = entry.second;
        if (mapping.table == table) {
            vec.push_back(mapping);
        }
    }
    return vec;
}

Mapping
AttributeMapper::mapping(const QString& attribute) const {
    MappingMap::const_iterator i = mappings_.find(attribute);
    if (i == mappings_.end()) {
        QString err = QString::fromUtf8("no mapping for attribute:") + 
                      attribute;
        throw lookup_error(err.toUtf8().constData());
    }
    return i->second;
}

} // namespace rdb
} // namespace brfc
