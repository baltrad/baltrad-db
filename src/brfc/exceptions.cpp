#include <brfc/exceptions.hpp>

#include <QtSql/QSqlError>

namespace brfc {

db_error::db_error(const QSqlError& error)
        : brfc_error(error.text().toStdString()) {
}

} // namespace brfc
