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

#include <brfc/rdb/BindMap.hpp>

#include <utility>

#include <brfc/exceptions.hpp>

namespace brfc {
namespace rdb {

BindMap::BindMap()
        : binds_() {

}

BindMap::BindMap(const BindMap& other)
        : binds_(other.binds_) {

}

BindMap&
BindMap::operator=(const BindMap& rhs) {
    if (this != &rhs) {
        binds_ = rhs.binds_;
    }
    return *this;
}

void
BindMap::add(const QString& name, const QVariant& value) {
    const map::value_type& pair = std::make_pair(name_to_placeholder(name),
                                                 value);
    if (not binds_.insert(pair).second)
        throw duplicate_entry(name.toStdString());
}

void
BindMap::add(const QString& name, const Variant& value) {
    add(name, value.to_qvariant());
}

bool
BindMap::has(const QString& name) const {
    return binds_.find(name_to_placeholder(name)) != binds_.end();
}

const QVariant&
BindMap::get(const QString& name) const {
    map::const_iterator iter = binds_.find(name_to_placeholder(name));
    if (iter == binds_.end())
        throw lookup_error(name.toStdString());
    return iter->second;
}

const QVariant&
BindMap::get(const QString& name, const QVariant& default_) const {
    try {
        return get(name);
    } catch (const lookup_error&) {
        return default_;
    }
}

QString
BindMap::name_to_placeholder(const QString& name) const {
    QString placeholder(name);
    if (not placeholder.startsWith(':'))
        placeholder.prepend(':');
    return placeholder;
}

bool
BindMap::remove(const QString& name) {
    return binds_.erase(name_to_placeholder(name));
}

} // namespace rdb
} // namespace brfc
