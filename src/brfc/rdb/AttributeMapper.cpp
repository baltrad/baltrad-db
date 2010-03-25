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
