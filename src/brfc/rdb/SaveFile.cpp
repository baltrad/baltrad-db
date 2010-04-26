#include <brfc/rdb/SaveFile.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtSql/QSqlError>

#include <brfc/FileHasher.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>

#include <brfc/rdb/RelationalDatabase.hpp>

namespace brfc {
namespace rdb {

SaveFile::SaveFile(RelationalDatabase* rdb)
        : rdb_(rdb)
        , group_id_cache_(rdb)
        , save_attribute_(rdb, &group_id_cache_)
        , save_group_(rdb, &group_id_cache_) {
}

void
SaveFile::operator()(const oh5::Group& group) {
    save_group_(group);
}

void
SaveFile::operator()(const oh5::Attribute& attribute) {
    save_attribute_(attribute);
}

long long
SaveFile::operator()(const oh5::File& file,
                     const QString& proposed_filename,
                     unsigned int filename_version) {
    QStringList columns, binds;
    columns.append("source_id");
    columns.append("unique_id");

    const MappingVector& special = rdb_->mapper().specializations_on("bdb_files");
    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: get rid of this
        if (mapping.attribute == "file_id") 
            continue;
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const QString& column, columns) {
        binds.append(":" + column);
    }

    QSqlQuery qry(rdb_->connection());
    QString qrystr("INSERT INTO bdb_files(" + columns.join(", ") +
                   ", hash_type, proposed_filename, filename_version) "
                   "VALUES(" + binds.join(", ") +
                   ", :hash_type, :proposed_filename, :filename_version)");
    if (rdb_->supports_returning())
        qrystr += " RETURNING id";
    qry.prepare(qrystr);

    if (not file.source())
        throw db_error("no Source associated with File");

    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: do something about it, we can't continue adding here
        if (mapping.attribute == "path" || mapping.attribute == "file_id")
            continue;
        const QVariant& value = 
                file.root()->child_attribute(mapping.attribute)->value().to_qvariant();
        qry.bindValue(":" + mapping.column, value);
    }

    qry.bindValue(":path", file.path());
    qry.bindValue(":hash_type", rdb_->file_hasher().name());
    qry.bindValue(":unique_id", rdb_->file_hasher().hash(file));
    qry.bindValue(":source_id", rdb_->db_id(*file.source()));
    qry.bindValue(":proposed_filename", proposed_filename);
    qry.bindValue(":filename_version", filename_version);

    if (not qry.exec())
        throw db_error(qry.lastError());

    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        visit(node, *this);
    }

    if (rdb_->supports_returning()) {
        qry.first();
        return qry.value(0).toLongLong();
    } else {
        return qry.lastInsertId().toLongLong();
    }
}



} // namespace rdb
} // namespace brfc
