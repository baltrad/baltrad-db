#include <brfc/AttributeSpecs.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Converter.hpp>

namespace brfc {

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
AttributeSpecs::add(const std::string& name, const std::string& type) {
    if (has(name))
        throw duplicate_entry(name);
    ConverterMap::const_iterator iter = converters_.find(type);
    if (iter == converters_.end())
        throw lookup_error(type);
    specs_.insert(std::make_pair(name, type));
}

bool
AttributeSpecs::has(const std::string& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    return iter != specs_.end();
}

const Converter&
AttributeSpecs::converter(const std::string& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    if (iter == specs_.end())
        throw lookup_error(name);
    ConverterMap::const_iterator i = converters_.find(iter->second);
    return *(i->second);
}

} // namespace brfc
