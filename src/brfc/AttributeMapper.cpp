#include <brfc/AttributeMapper.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Converter.hpp>

#include <boost/foreach.hpp>

namespace brfc {

AttributeMapper::AttributeMapper()
        : specs_()
        , converters_() {
    converters_.insert(std::make_pair("int", new IntConverter()));
    converters_.insert(std::make_pair("real", new RealConverter()));
    converters_.insert(std::make_pair("string", new StringConverter()));
    converters_.insert(std::make_pair("date", new DateConverter()));
    converters_.insert(std::make_pair("time", new TimeConverter()));
    converters_.insert(std::make_pair("bool", new BoolConverter()));
    converters_.insert(std::make_pair("sequence", new SequenceConverter()));
}

AttributeMapper::~AttributeMapper() {
    BOOST_FOREACH(const ConverterMap::value_type& val, converters_) {
        delete val.second;
    }
}

void
AttributeMapper::add_spec(int id,
                          const std::string& name,
                          const std::string& converter,
                          const std::string& table,
                          const std::string& column) {
    specs_.insert(std::make_pair(name,
                                 AttributeSpec(id,
                                               name,
                                               *converters_[converter],
                                               table,
                                               column)));
}

const AttributeSpec&
AttributeMapper::spec(const std::string& attribute) const {
    AttributeSpecMap::const_iterator i = specs_.find(attribute);
    if (i == specs_.end())
        throw lookup_error("attribute not found: " + attribute);
    return i->second;
}

bool
AttributeMapper::has(const std::string& attribute) const {
    return specs_.find(attribute) != specs_.end();
}

bool
AttributeMapper::is_specialized(const std::string& attribute) const {
    return spec(attribute).table.find("attribute_values") != 0;
}

MappingVector
AttributeMapper::specializations_on(const std::string& table) const {
    MappingVector vec;
    BOOST_FOREACH(const AttributeSpecMap::value_type& entry, specs_) {
        const AttributeSpec& spec = entry.second;
        if (spec.table == table) {
            vec.push_back(Mapping(spec.name, spec.table, spec.column));
        }
    }
    return vec;
}

Mapping
AttributeMapper::mapping(const std::string& attribute) const {
    const AttributeSpec& s = spec(attribute);
    return Mapping(s.name, s.table, s.column);
}

const Converter&
AttributeMapper::converter(const std::string& attribute) const {
    return spec(attribute).converter;
}

} // namespace brfc
