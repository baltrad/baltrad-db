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

#include <brfc/oh5/File.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDate>
#include <QtCore/QString>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Time.hpp>
#include <brfc/Variant.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/FileLoader.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

File::File()
        : enable_shared_from_this<File>()
        , root_(make_shared<RootGroup>())
        , path_()
        , source_() {
}

File::~File() {

}

shared_ptr<File>
File::create() {
    shared_ptr<File> f(new File());
    f->root_->file(f);
    return f;
}

shared_ptr<File>
File::from_filesystem(const QString& path, const AttributeSpecs& specs) {
    return FileLoader(&specs).load(path);
}

shared_ptr<File>
File::minimal(const QString& object,
              const QDate& date,
              const Time& time,
              const QString& source,
              const QString& version) {
    shared_ptr<File> f = create();
    f->root_->add_child(make_shared<Attribute>("Conventions",
                                               Variant("ODIM_H5/V2_0")));
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    f->root_->add_child(what);
    what->add_child(make_shared<Attribute>("object", Variant(object)));
    what->add_child(make_shared<Attribute>("version", Variant(version)));
    what->add_child(make_shared<Attribute>("date", Variant(date)));
    what->add_child(make_shared<Attribute>("time", Variant(time)));
    what->add_child(make_shared<Attribute>("source", Variant(source)));
    return f;
}

void
File::source(shared_ptr<Source> source) {
    source_ = source;
}

QString
File::what_object() const {
    return root_->child_attribute("what/object")->value().string();
}

QDate
File::what_date() const {
    return root_->child_attribute("what/date")->value().date();
}

Time
File::what_time() const {
    return root_->child_attribute("what/time")->value().time();
}

QString
File::what_source() const {
    return root_->child_attribute("what/source")->value().string();
}

QString
File::name() const {
    return path().section('/', -1);
}

File::StringVector
File::invalid_attribute_paths() const {
    StringVector paths;
    BOOST_FOREACH(shared_ptr<const Attribute> attr, invalid_attributes()) {
        paths.push_back(attr->path());
    }
    return paths;
}

File::ConstAttributeVector
File::invalid_attributes() const {
    ConstAttributeVector attrs;

    const Attribute* attr = 0;
    BOOST_FOREACH(const Node& node, *root_) {
        if ((attr = dynamic_cast<const Attribute*>(&node)) == 0)
            continue;
        if (not attr->is_valid())
            attrs.push_back(attr->shared_from_this());
    }
    
    return attrs;
}

File::AttributeVector
File::invalid_attributes() {
    // implement through const
    const File* self = const_cast<const File*>(this);

    ConstAttributeVector const_attrs = self->invalid_attributes();

    AttributeVector attrs;
    attrs.reserve(const_attrs.size());

    BOOST_FOREACH(shared_ptr<const Attribute> const_attr, const_attrs) {
        attrs.push_back(const_pointer_cast<Attribute>(const_attr));
    }

    return attrs;
}

shared_ptr<Group>
File::group(const QString& path) {
    const File* self = const_cast<const File*>(this);
    return const_pointer_cast<Group>(self->group(path));
}

shared_ptr<const Group>
File::group(const QString& path) const {
    QString path_copy = path;
    if (path_copy.startsWith("/"))
        path_copy.remove(0, 1);
    if (path_copy == "")
        return root_;
    shared_ptr<const Node> node = root_->child_by_path(path_copy);
    return const_pointer_cast<Group>(dynamic_pointer_cast<const Group>(node));
}

} // namespace oh5
} // namepsace brfc
