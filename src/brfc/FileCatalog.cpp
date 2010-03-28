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

#include <brfc/FileCatalog.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/Database.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>

#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDir>

namespace brfc {
    
FileCatalog::FileCatalog(const QString& dsn,
                         const QString& storage)
        : db_(new rdb::RelationalDatabase(dsn))
        , specs_()
        , namer_(new DefaultFileNamer())
        , storage_(storage) {
    check_storage();
    rdb::RelationalDatabase* rdb =
        static_cast<rdb::RelationalDatabase*>(db_.get());
    specs_.reset(new oh5::AttributeSpecs(rdb->specs()));
}

FileCatalog::FileCatalog(shared_ptr<Database> db,
                         shared_ptr<oh5::AttributeSpecs> specs,
                         shared_ptr<FileNamer> namer,
                         const QString& storage)
        : db_(db)
        , specs_(specs)
        , namer_(namer)
        , storage_(storage) {
    check_storage();
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::check_storage() const {
    if (not storage_.isAbsolute())
        throw fs_error("storage must be an absolute path");
    if (not storage_.exists())
        throw fs_error("storage does not exist");
}

bool
FileCatalog::is_cataloged(const QString& path) const {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path, *specs_);
    return is_cataloged(*f);
}

bool
FileCatalog::is_cataloged(const oh5::File& f) const {
    return db_->has_file(f);
}

shared_ptr<const oh5::File>
FileCatalog::catalog(const QString& path) {
    shared_ptr<oh5::File> f = oh5::File::from_filesystem(path, *specs_);
    
    f->source(db_->load_source(f->what_source()));

    if (is_cataloged(*f))
        throw duplicate_entry(path.toUtf8().constData());
    
    QString proposed_name = namer_->name(*f);

    if (QDir::isAbsolutePath(proposed_name))
        throw std::runtime_error("namer must not return absolute paths");
    
    unsigned int name_version = db_->next_filename_version(proposed_name);
    
    QString target = FileNamer::inject_version(proposed_name, name_version);
    target = QDir::cleanPath(storage_.absoluteFilePath(target));
    f->path(target);

    db_->begin();
    // try saving to database
    try {
        long long id = db_->save_file(*f, proposed_name, name_version);
        f->db_id(id);
    } catch (const db_error& e) {
        db_->rollback();
        throw;
    }
    // database save OK, try copying to new location
    if (not QFile::copy(path, target)) {
        db_->rollback();
        QString err = QString::fromUtf8("could not copy ") + path +
                      QString::fromUtf8(" to ") + target;
        throw fs_error(err.toUtf8().constData());
    } else {
        db_->commit();
    }
    return f;
}

void
FileCatalog::remove(const QString& path) {
    db_->begin();
    db_->remove_file(path);
    //XXX: what about when it exists in db but not in fs?
    if (not QFile::remove(path)) {
        db_->rollback();
        QString err = QString::fromUtf8("could not remove ") + path;
        throw fs_error(err.toUtf8().constData());
    } else {
        db_->commit();
    }
}

void
FileCatalog::clear() {
    Query q = query();
    q.fetch(expr::Attribute::create("path"));
    shared_ptr<ResultSet> r = q.execute();
    while (r->next()) {
        remove(r->string(0));
    }
}

Query
FileCatalog::query() const {
    return Query(db_.get());
}

} // namespace brfc
