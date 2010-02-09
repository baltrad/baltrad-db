#include <brfc/FileCatalog.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/File.hpp>
#include <brfc/Database.hpp>
#include <brfc/RelationalDatabase.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDir>

namespace brfc {
    
DefaultFileNamer::DefaultFileNamer(const std::string& path)
        : path_(path) {
    QDir dir_(path.c_str());
    if (not dir_.isAbsolute())
        throw fs_error("storage must be an absolute path");
    if (not dir_.exists())
        throw fs_error("storage does not exist");
}

std::string
DefaultFileNamer::do_name(const File& f) const {
    return path_ + "/" + f.unique_identifier() + ".h5";
}

FileCatalog::FileCatalog(const std::string& dsn,
                         const std::string& storage)
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
FileCatalog::is_cataloged(const std::string& path) const {
    File f(path, *specs_);
    return is_cataloged(f);
}

bool
FileCatalog::is_cataloged(const File& f) const {
    return db_->has_file(f);
}

shared_ptr<File>
FileCatalog::catalog(const std::string& path) {
    shared_ptr<File> f(new File(path, *specs_));

    if (is_cataloged(*f))
        throw duplicate_entry(path);
    
    std::string target = namer_->name(*f);

    if (not QDir::isAbsolutePath(target.c_str()))
        throw std::runtime_error("namer must return absolute paths");

    f->path(target);

    db_->begin();
    // try saving to database
    try {
        db_->save_file(target.c_str(), *f);
    } catch (const db_error& e) {
        db_->rollback();
        throw;
    }
    // database save OK, try copying to new location
    if (not QFile::copy(path.c_str(), target.c_str())) {
        db_->rollback();
        throw fs_error("could not copy " + path + " to " + target);
    } else {
        db_->commit();
    }
    return f;
}

void
FileCatalog::remove(const std::string& path) {
    db_->begin();
    db_->remove_file(path.c_str());
    //XXX: what about when it exists in db but not in fs?
    if (not QFile::remove(path.c_str())) {
        db_->rollback();
        throw fs_error("could not remove " + path);
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
