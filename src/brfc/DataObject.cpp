#include <brfc/DataObject.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <QtCore/QStringList>

namespace brfc {

DataObject::DataObject(const QString& name,
                       const DataObject* parent)
        : name_(name)
        , parent_(parent)
        , children_()
        , attrs_()
        , file_(parent->file_) {
    BRFC_ASSERT(not name.contains('/'));
}

DataObject::~DataObject() {

}

DataObject&
DataObject::child(const QString& name, bool create) {
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

QString
DataObject::path() const {
    QStringList names;
    const DataObject* dobj = this;
    while (dobj != 0) {
        names.push_front(dobj->name());
        dobj = dobj->parent();
    }
    return names.join("/");
}

DataObject&
DataObject::add_child(const QString& name) {
    children_.push_back(shared_ptr<DataObject>(new DataObject(name, this)));
    return *children_.back();
}

void
DataObject::add_attribute(const QString& name,
                          const Variant& value) {
    BOOST_FOREACH(shared_ptr<Attribute> attr, attrs_) {
        if (attr->name() == name) {
            throw duplicate_entry(name.toUtf8().constData());
        }
    }
    attrs_.push_back(shared_ptr<Attribute>(new Attribute(name, value, this)));
}

bool
DataObject::has_attribute(const QString& name) const {
    try {
        attribute(name);
    } catch (const std::runtime_error& e) {
        return false;
    }
    return true;
}

const Attribute&
DataObject::attribute(const QString& name) const {
    DataObject* self = const_cast<DataObject*>(this);
    return self->attribute(name);
}

Attribute&
DataObject::attribute(const QString& name) {
    AttributeVector::iterator i;
    BOOST_FOREACH(shared_ptr<Attribute> attr, attrs_) {
        if (attr->name() == name) {
            return *attr;
        }
    }
    QString msg = QString::fromUtf8("no such attribute: ") + name;
    throw lookup_error(msg.toUtf8().constData());
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
