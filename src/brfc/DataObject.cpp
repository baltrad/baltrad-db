#include <brfc/DataObject.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <algorithm>

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
    BOOST_FOREACH(shared_ptr<DataObject> child, children_) {
        if (child->name() == name) {
            return *child;
        }
    }

    if (create) {
        return add_child(name);
    } else {
        throw lookup_error("no child");
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
    children_.push_back(shared_ptr<DataObject>(new DataObject(name, this)));
    return *children_.back();
}

void
DataObject::add_attribute(const std::string& name,
                          const Variant& value) {
    BOOST_FOREACH(shared_ptr<Attribute> attr, attrs_) {
        if (attr->name() == name) {
            throw duplicate_entry(name);
        }
    }
    attrs_.push_back(shared_ptr<Attribute>(new Attribute(name, value, this)));
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
    AttributeVector::iterator i;
    BOOST_FOREACH(shared_ptr<Attribute> attr, attrs_) {
        if (attr->name() == name) {
            return *attr;
        }
    }
    throw lookup_error("no such attribute: " + name);
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
