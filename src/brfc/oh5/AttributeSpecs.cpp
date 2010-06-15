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

#include <brfc/oh5/AttributeSpecs.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/oh5/Converter.hpp>

namespace brfc {
namespace oh5 {

AttributeSpecs::AttributeSpecs()
         : specs_() {
}

AttributeSpecs::AttributeSpecs(const AttributeSpecs& other)
        : specs_(other.specs_) {

}

AttributeSpecs::~AttributeSpecs() {

}

void
AttributeSpecs::add(const AttributeSpec& spec) {
    if (has(spec.name))
        throw duplicate_entry(spec.name.to_utf8());
    specs_.insert(std::make_pair(spec.name, spec));
}

bool
AttributeSpecs::has(const String& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    return iter != specs_.end();
}

const AttributeSpec&
AttributeSpecs::get(const String& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    if (iter == specs_.end())
        throw lookup_error(name.to_utf8());
    return iter->second;
}

} // namespace oh5
} // namespace brfc
