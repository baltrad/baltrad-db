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
        throw duplicate_entry(spec.name.toUtf8().constData());
    ConverterMap::const_iterator iter = converters_.find(spec.type);
    if (iter == converters_.end())
        throw lookup_error(spec.type.toUtf8().constData());
    specs_.insert(std::make_pair(spec.name, spec));
}

bool
AttributeSpecs::has(const QString& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    return iter != specs_.end();
}

const AttributeSpec&
AttributeSpecs::get(const QString& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    if (iter == specs_.end())
        throw lookup_error(name.toUtf8().constData());
    return iter->second;
}

const Converter&
AttributeSpecs::converter(const QString& name) const {
    AttributeSpecMap::const_iterator iter = specs_.find(name);
    if (iter == specs_.end())
        throw lookup_error(name.toUtf8().constData());
    ConverterMap::const_iterator i = converters_.find(iter->second.type);
    return *(i->second);
}

} // namespace oh5
} // namespace brfc
