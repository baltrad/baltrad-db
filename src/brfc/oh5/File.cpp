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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/oh5/File.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QByteArray>
#include <QtCore/QDate>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTime>
#include <QtCore/QCryptographicHash>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
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
        : root_(make_shared<RootGroup>())
        , path_()
        , source_()
        , db_id_(0) {
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
              const QTime& time,
              const QString& source,
              const QString& version) {
    shared_ptr<File> f = create();
    f->root_->add_child(make_shared<Attribute>("Conventions",
                                               Variant("ODIM_H5/V2_0"),
                                               true));
    shared_ptr<AttributeGroup> what = make_shared<AttributeGroup>("what");
    f->root_->add_child(what);
    what->add_child(make_shared<Attribute>("object", Variant(object)));
    what->add_child(make_shared<Attribute>("version", Variant(version)));
    what->add_child(make_shared<Attribute>("date", Variant(date)));
    what->add_child(make_shared<Attribute>("time", Variant(time)));
    what->add_child(make_shared<Attribute>("source", Variant(source), true));
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

QTime
File::what_time() const {
    return root_->child_attribute("what/time")->value().time();
}

QString
File::what_source() const {
    return root_->child_attribute("what/source")->value().string();
}

QString
File::unique_identifier() const {
    if (not source_) {
        //XXX: needs a better exception type
        throw value_error("can't form unique_id: not associated with source");
    }
    
    QStringList strs(source_->node_id());
    const Attribute* attr = 0;
    BOOST_FOREACH(const Node& node, *root_) {
        attr = dynamic_cast<const Attribute*>(&node);
        if (attr and not attr->ignore_in_hash()) {
            strs.append(attr->to_string());
        }
    }
    strs.sort(); // ensure same order

    QByteArray bytes = strs.join("").toUtf8();
    QByteArray hash = QCryptographicHash::hash(bytes,
                                               QCryptographicHash::Sha1);
    return QString::fromAscii(hash.toHex());
}

File::StringVector
File::ignored_attributes() const {
    /* XXX: should rename this to ignored_attribute_paths()
     * and create a method ignored_attributes returning a vector of
     * attribute pointers
     */
    StringVector paths;

    const Attribute* attr = 0;
    BOOST_FOREACH(const Node& node, *root_) {
        if ((attr = dynamic_cast<const Attribute*>(&node)) == 0)
            continue;
        if (not attr->is_valid())
            paths.push_back(attr->path());
    }
    
    return paths;
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
