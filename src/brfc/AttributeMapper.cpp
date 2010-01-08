#include <brfc/AttributeMapper.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Converter.hpp>

#include <boost/foreach.hpp>

namespace brfc {

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
        throw duplicate_entry(mapping.attribute);
    mappings_.insert(std::make_pair(mapping.attribute, mapping));
}

bool
AttributeMapper::has(const std::string& attribute) const {
    return mappings_.find(attribute) != mappings_.end();
}

bool
AttributeMapper::is_specialized(const std::string& attribute) const {
    return mapping(attribute).table.find("attribute_values") != 0;
}

MappingVector
AttributeMapper::specializations_on(const std::string& table) const {
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
AttributeMapper::mapping(const std::string& attribute) const {
    MappingMap::const_iterator i = mappings_.find(attribute);
    if (i == mappings_.end())
        throw lookup_error("no mapping for attribute: " + attribute);
    return i->second;
}

} // namespace brfc
