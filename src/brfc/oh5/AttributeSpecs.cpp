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
         : specs_()
         , converters_() {
    converters_["int"] = shared_ptr<Converter>(new IntConverter());
    converters_["real"] = shared_ptr<Converter>(new RealConverter());
    converters_["string"] = shared_ptr<Converter>(new StringConverter());
    converters_["date"] = shared_ptr<Converter>(new DateConverter());
    converters_["time"] = shared_ptr<Converter>(new TimeConverter());
    converters_["bool"] = shared_ptr<Converter>(new BoolConverter());
    converters_["sequence"] = shared_ptr<Converter>(new SequenceConverter());
}

AttributeSpecs::AttributeSpecs(const AttributeSpecs& other)
        : specs_(other.specs_)
        , converters_(other.converters_) {

}

AttributeSpecs::~AttributeSpecs() {

}

void
AttributeSpecs::add(const AttributeSpec& spec) {
    if (has(spec.name))
        throw duplicate_entry(spec.name.to_utf8());
    ConverterMap::const_iterator iter = converters_.find(spec.type);
    if (iter == converters_.end())
        throw lookup_error(spec.type.to_utf8());
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

const Converter&
AttributeSpecs::converter(const String& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    if (iter == specs_.end())
        throw lookup_error(name.to_utf8());
    ConverterMap::const_iterator i = converters_.find(iter->second.type);
    return *(i->second);
}

} // namespace oh5
} // namespace brfc
