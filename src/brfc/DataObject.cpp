#include <brfc/DataObject.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/algorithm/string/join.hpp>

namespace brfc {

DataObject::DataObject(const std::string& name,
                       const DataObject* parent)
        : name_(name)
        , parent_(parent)
        , children_()
        , attrs_()
        , file_(parent->file_) {
    // assert '/' not in name
    BRFC_ASSERT(std::find(name.begin(), name.end(), '/') == name.end());
}

DataObject::~DataObject() {

}

DataObject&
DataObject::child(const std::string& name, bool create) {
    using namespace boost::lambda;

    ChildVector::iterator i;
    i = std::find_if(children_.begin(), children_.end(),
                     bind(&DataObject::name_, _1) == name);

    if (i == children_.end()) {
        if (create) {
            return add_child(name);
        } else {
            throw lookup_error("no child");
        }
    } else {
        return *i;
    }
}

std::string
DataObject::path() const {
    std::deque<std::string> names;
    const DataObject* dobj = this;
    while (dobj != 0) {
        names.push_front(dobj->name());
        dobj = dobj->parent();
    }
    return boost::join(names, "/");
}

DataObject&
DataObject::add_child(const std::string& name) {
    children_.push_back(new DataObject(name, this));
    return children_.back();
}

void
DataObject::add_attribute(const std::string& name,
                          const QVariant& value) {
    attrs_.push_back(new Attribute(name, value, this));
}

bool
DataObject::has_attribute(const std::string& name) const {
    try {
        attribute(name);
    } catch (const std::runtime_error& e) {
        return false;
    }
    return true;
}

const Attribute&
DataObject::attribute(const std::string& name) const {
    DataObject* self = const_cast<DataObject*>(this);
    return self->attribute(name);
}

Attribute&
DataObject::attribute(const std::string& name) {
    using namespace boost::lambda;

    AttributeVector::iterator i;
    i = std::find_if(attrs_.begin(), attrs_.end(),
                     bind(&Attribute::name, _1) == name);
    if (i == attrs_.end()) {
        throw lookup_error("no such attribute: " + name);
    }
    return *i;
}

DataObject::iterator
DataObject::begin() {
    return iterator(this);
}

DataObject::iterator
DataObject::end() {
    return iterator();
}

DataObject::const_iterator
DataObject::begin() const {
    return const_iterator(this);
}

DataObject::const_iterator
DataObject::end() const {
    return const_iterator();
}

} // namespace brfc
