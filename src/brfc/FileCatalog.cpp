#include <brfc/FileCatalog.hpp>

#include <brfc/exceptions.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/File.hpp>
#include <brfc/Database.hpp>
#include <brfc/Query.hpp>
#include <brfc/ResultSet.hpp>

#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/Factory.hpp>
#include <brfc/expr/Literal.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QDir>

namespace brfc {

FileCatalog::FileCatalog(const std::string& dsn,
                         const std::string& storage)
        : mapper_(new AttributeMapper())
        , db_(new Database(dsn))
        , storage_(new QDir(storage.c_str())) {
    
    if (!storage_->isAbsolute())
        throw fs_error("storage must be an absolute path");
    if (!storage_->exists())
        throw fs_error("storage does not exist");
    bootstrap();
}

FileCatalog::~FileCatalog() {

}

void
FileCatalog::bootstrap() {
    db_->populate_attribute_mapper(*mapper_);
}

bool
FileCatalog::is_cataloged(const std::string& path) const {
    File f(path, *mapper_);
    return is_cataloged(f);
}

bool
FileCatalog::is_cataloged(const File& f) const {
    Query q = query();
    expr::Factory xpr;
    expr::AttributePtr path_attr = xpr.attribute("path");
    q.fetch(path_attr).filter(path_attr->eq(xpr.string(f.path())));
    ResultSet r = q.execute();
    return r.next(); // has at least 1 row
}

std::string
FileCatalog::catalog(const std::string& path) {
    File f(path, *mapper_);

    if (is_cataloged(f))
        throw duplicate_entry(path);

    db_->begin();
    // try saving to database
    try {
        db_->save_recurse(f, *mapper_);
    } catch (const db_error& e) {
        db_->rollback();
        throw;
    }
    // database save OK, try copying to new location
    QString target = storage_->absoluteFilePath(f.path().c_str());
    if (!QFile::copy(path.c_str(), target)) {
        db_->rollback();
        throw fs_error("could not copy " + std::string(path) +
                       " to " + target.toStdString());
    } else {
        db_->commit();
    }
    return f.path();
}

void
FileCatalog::remove(const std::string& path) {
    QString target = storage_->absoluteFilePath(path.c_str());
    db_->begin();
    db_->remove_file(path.c_str());
    //XXX: what about when it exists in db but not in fs?
    if (!QFile::remove(target)) {
        db_->rollback();
        throw fs_error("could not remove " + target.toStdString());
    } else {
        db_->commit();
    }
}

void
FileCatalog::clear() {
    Query q = query();
    q.fetch(expr::Attribute::create("path"));
    ResultSet r = q.execute();
    while (r.next()) {
        remove(r.string(0));
    }
}

Query
FileCatalog::query() const {
    return Query(db_.get(), mapper_.get());
}

} // namespace brfc
