#include <brfc/TestRDB.hpp>

#include <brfc/exceptions.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace brfc {

TestRDB::TestRDB(const std::string& dsn, const std::string& schema_dir)
        : RelationalDatabase(dsn)
        , schema_dir_(QString::fromUtf8(schema_dir.c_str())) {
    schema_dir_.makeAbsolute();
}

void
TestRDB::create() {
    exec_queries_from(QString("create.sql"));
    populate_mapper_and_specs();
}

void
TestRDB::drop() {
    exec_queries_from(QString("drop.sql"));
}

void
TestRDB::clean() {
    connection().exec(QString("DELETE FROM files"));
}

QStringList
TestRDB::load_queries(const QString& filename) {
    QString path = schema_dir_.path() + QString("/") + dialect() + QString("/") + filename;
    QFile file(path);
    if (not file.open(QIODevice::ReadOnly)) {
        throw fs_error("could not open file: " + path.toStdString());
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    const QString content = stream.readAll();
    return content.split(";\n", QString::SkipEmptyParts);
}

void
TestRDB::exec_queries_from(const QString& file) {
    const QStringList& queries = load_queries(file);
    begin();
    QSqlQuery query(connection());
    BOOST_FOREACH(const QString& stmt, queries) {
        if (not query.exec(stmt)) {
            rollback();
            throw db_error(query.lastError().text().toStdString()); 
        }
    }
    commit();
}

} // namespace brfc
