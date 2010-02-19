#include <brfc/FileCatalog.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/Database.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/DefaultFileNamer.hpp>
#include <brfc/File.hpp>
#include <brfc/Query.hpp>
#include <brfc/RelationalDatabase.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDir>

namespace brfc {
    
FileCatalog::FileCatalog(const QString& dsn,
                         const QString& storage)
        : db_(new RelationalDatabase(dsn))
        , specs_()
        , namer_(new DefaultFileNamer(storage)) {
    RelationalDatabase* rdb = static_cast<RelationalDatabase*>(db_.get());
    specs_.reset(new AttributeSpecs(rdb->specs()));
}

FileCatalog::FileCatalog(shared_ptr<Database> db,
                         shared_ptr<AttributeSpecs> specs,
                         shared_ptr<FileNamer> namer)
        : db_(db)
        , specs_(specs)
        , namer_(namer) {

}

FileCatalog::~FileCatalog() {

}

bool
FileCatalog::is_cataloged(const QString& path) const {
    File f(path, *specs_);
    return is_cataloged(f);
}

bool
FileCatalog::is_cataloged(const File& f) const {
    return db_->has_file(f);
}

shared_ptr<File>
FileCatalog::catalog(const QString& path) {
    shared_ptr<File> f(new File(path, *specs_));
    
    QString src = f->root().attribute("what/source").value().string();
    f->source(db_->load_source(src));

    if (is_cataloged(*f))
        throw duplicate_entry(path.toUtf8().constData());
    
    QString target = namer_->name(*f);

    if (not QDir::isAbsolutePath(target))
        throw std::runtime_error("namer must return absolute paths");

    f->path(target);

    db_->begin();
    // try saving to database
    try {
        long long id = db_->save_file(target, *f);
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
