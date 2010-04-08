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

#include <brfc/rdb/RelationalDatabase.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/Query.hpp>
#include <brfc/Variant.hpp>
#include <brfc/visit.hpp>
#include <brfc/FileHasher.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/AttrReplace.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Select.hpp>

#include <brfc/oh5/Attribute.hpp>
#include <brfc/oh5/AttributeGroup.hpp>
#include <brfc/oh5/AttributeSpecs.hpp>
#include <brfc/oh5/File.hpp>
#include <brfc/oh5/RootGroup.hpp>
#include <brfc/oh5/SourceCentre.hpp>
#include <brfc/oh5/SourceRadar.hpp>

#include <brfc/rdb/AttributeMapper.hpp>
#include <brfc/rdb/RelationalResultSet.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include <map>

namespace brfc {
namespace rdb {

namespace {

class GroupSaver {
  public:
    GroupSaver(RelationalDatabase* rdb)
            : rdb_(rdb)
            , qry_(rdb->connection())
            , special_(rdb->mapper().specializations_on("groups")) {
        QStringList columns, binds; 
        columns.append("parent_id");
        columns.append("file_id");
        columns.append("name");
        BOOST_FOREACH(const Mapping& mapping, special_) {
            columns.append(mapping.column);
        }
        BOOST_FOREACH(const QString& column, columns) {
            binds.append(":" + column);
        }
        QString qrystr("INSERT INTO groups(" + columns.join(", ") +
                       ") VALUES(" + binds.join(", ") + ")");
        if (rdb->supports_returning())
            qrystr += " RETURNING id";
        if (not qry_.prepare(qrystr))
            throw db_error(qry_.lastError());
    }
    
    void operator()(const oh5::Group& group) {
        bind_plain(group);
        bind_specializations(group);

        if (not qry_.exec())
            throw db_error(qry_.lastError());
        group.db_id(last_id());
    }

  private:
    void bind_plain(const oh5::Group& group) {
        QVariant file_id, parent_id;
        if (group.parent()) {
            shared_ptr<const oh5::Group> parent =
                dynamic_pointer_cast<const oh5::Group>(group.parent());
            parent_id = parent->db_id();
        }
        if (group.file())
            file_id = group.file()->db_id();

        qry_.bindValue(":parent_id", parent_id);
        qry_.bindValue(":file_id", file_id);
        qry_.bindValue(":name", group.name());
    }

    void bind_specializations(const oh5::Group& group) {
        BOOST_FOREACH(const Mapping& mapping, special_) {
            QVariant val;
            shared_ptr<const oh5::Attribute> attr =
                group.child_attribute(mapping.attribute);
            if (attr)
                val = attr->value().to_qvariant();
            qry_.bindValue(":" + mapping.column, val);
        }
    }

    long long last_id() {
        if (rdb_->supports_returning()) {
            qry_.first();
            return qry_.value(0).toLongLong();
        } else {
            return qry_.lastInsertId().toLongLong();
        }
    }

    RelationalDatabase* rdb_;
    QSqlQuery qry_;
    MappingVector special_;
};

class AttributeSaver {
  public:
    AttributeSaver(RelationalDatabase* rdb)
            : rdb_(rdb)
            , mapper_(&rdb->mapper())
            , queries_() {
    }

    void operator()(const oh5::Attribute& attr) {
        // ignore invalid attributes (XXX: should be saved to separate table)
        if (not attr.is_valid())
            return;

        // ignore specialized attributes
        if (mapper_->is_specialized(attr.full_name()))
            return;
        
        const Mapping& mapping = mapper_->mapping(attr.full_name());

        QSqlQuery& qry = query_by_table(mapping.table);
        QVariant grp_id;
        if (attr.parent_group())
            grp_id = attr.parent_group()->db_id();
        qry.bindValue("group_id", grp_id);
        qry.bindValue("attribute_id", mapping.id);
        qry.bindValue("value", attr.value().to_qvariant());

        if (not qry.exec())
            throw db_error(qry.lastError());
    }

  private:
    typedef std::map<QString, QSqlQuery> QueryMap;

    QSqlQuery& query_by_table(const QString& table) {
        QueryMap::iterator iter = queries_.find(table);
        if (iter == queries_.end()) {
            QSqlQuery qry(rdb_->connection()); 
            qry.prepare("INSERT INTO " + table +
                        "(group_id, attribute_id, value) " +
                        "VALUES(:group_id, :attribute_id, :value)");
            QueryMap::value_type val(table, qry);
            iter = queries_.insert(val).first;
        }
        return iter->second;
    }

    RelationalDatabase* rdb_;
    const AttributeMapper* mapper_;
    QueryMap queries_;
};

class SaveVisitor {
  public:
    SaveVisitor(RelationalDatabase* db)
            : save_attribute_(db)
            , save_group_(db) {
    }

    typedef mpl::vector<const oh5::AttributeGroup,
                        const oh5::Group,
                        const oh5::Attribute> accepted_types;

    void operator()(const oh5::AttributeGroup& group) {
        // no-op
    }

    void operator()(const oh5::Group& group) {
        save_group_(group);
    }
    
    void operator()(const oh5::Attribute& attribute) {
        save_attribute_(attribute);
    }

  private:
    AttributeSaver save_attribute_;
    GroupSaver save_group_;
};

void
QSqlDatabase_deleter(QSqlDatabase* db) {
    const QString& name = db->connectionName();
    db->close();
    delete db;
    ::QSqlDatabase::removeDatabase(name);
}

} // namespace anonymous


QString
RelationalDatabase::qt_engine(const QString& dialect) const {
    if (dialect == "postgresql") {
        return "QPSQL";
    } else if (dialect == "sqlite") {
        return "QSQLITE";
    } else {
        QString err = QString::fromUtf8("unsupported dialect: ") +
                      dialect;
        throw db_error(err.toUtf8().constData());
    }
}

RelationalDatabase::RelationalDatabase(const QString& dsn_)
        : sql_()
        , mapper_(new AttributeMapper())
        , specs_(new oh5::AttributeSpecs())
        , file_hasher_()
        , dialect_()
        , supports_returning_(false) {
        
    if (qapp_ == 0)
        init_qapp();

    QUrl dsn(dsn_);

    {
        QString name = QString("brfc-") + QString::number(connection_count_++);
        QString engine = qt_engine(dsn.scheme());
        const QSqlDatabase& tmp = ::QSqlDatabase::addDatabase(engine, name);
        sql_.reset(new QSqlDatabase(tmp), QSqlDatabase_deleter);
    }

    sql_->setHostName(dsn.host());
    sql_->setUserName(dsn.userName());
    sql_->setPassword(dsn.password());
    QString database = dsn.path();
    if (database.startsWith("/")) {
        database.remove(0, 1); // remove slash
    }
    sql_->setDatabaseName(database);
    if (!sql_->open())
        throw db_error(sql_->lastError());
    dialect_ = dsn.scheme();
    if (dialect_ == "postgresql")
        supports_returning_ = true;
    populate_mapper_and_specs();
}

void
RelationalDatabase::init_qapp() {
    // never gets destroyed
    qapp_ = new QCoreApplication(argc_, const_cast<char**>(argv_));
}

unsigned int RelationalDatabase::connection_count_ = 0;
int RelationalDatabase::argc_ = 1;
const char* RelationalDatabase::argv_[] = {"brfc"};
QCoreApplication* RelationalDatabase::qapp_ = 0;

RelationalDatabase::~RelationalDatabase() {

}

shared_ptr<const oh5::AttributeSpecs>
RelationalDatabase::specs() const {
    return specs_;
}

shared_ptr<oh5::AttributeSpecs>
RelationalDatabase::specs() {
    return specs_;
}

const AttributeMapper&
RelationalDatabase::mapper() const {
    return *mapper_;
}

void
RelationalDatabase::file_hasher(shared_ptr<FileHasher> hasher) {
    file_hasher_.swap(hasher);
}

void
RelationalDatabase::file_hasher(FileHasher* hasher) {
    file_hasher(shared_ptr<FileHasher>(hasher, no_delete));
}

void
RelationalDatabase::do_begin() {
    connection().transaction();
}

void
RelationalDatabase::do_rollback() {
    connection().rollback();
}

void
RelationalDatabase::do_commit() {
    connection().commit();
}

bool
RelationalDatabase::do_has_file(const oh5::File& file) {
    QSqlQuery query(connection());
    query.prepare("SELECT true FROM files WHERE unique_id = :unique_id");
    query.bindValue(":unique_id", file_hasher_->hash(file));
    if (!query.exec())
        throw db_error(query.lastError());
    return query.next(); // got a result row
}


long long
RelationalDatabase::do_save_file(const oh5::File& file,
                                 const QString& proposed_filename,
                                 unsigned int filename_version) {
    id_type file_id = save(file, proposed_filename, filename_version);
    file.db_id(file_id.toLongLong());

    SaveVisitor visitor(this);
    
    BOOST_FOREACH(const oh5::Node& node, *file.root()) {
        visit(node, visitor);
    }

    return file_id.toLongLong();
}

unsigned int
RelationalDatabase::do_next_filename_version(const QString& filename) {
    QSqlQuery qry(connection());
    qry.prepare("SELECT MAX(filename_version) + 1 "
                "FROM files "
                "WHERE proposed_filename = :filename");
    qry.bindValue(":filename", filename);
    if (!qry.exec())
        throw db_error(qry.lastError());
    qry.next();
    return qry.value(0).toUInt();
}

shared_ptr<ResultSet>
RelationalDatabase::do_query(const Query& query) {
    expr::SelectPtr select = expr::Select::create();

    BOOST_FOREACH(expr::AttributePtr expr, query.fetch()) {
        select->what(expr);
    }

    select->where(query.filter());
    select->distinct(query.distinct());

    expr::AttrReplace::replace(select, mapper_.get());
    expr::Compiler compiler;
    compiler.compile(*select);

    return this->query(compiler.compiled(), compiler.binds());
}

RelationalDatabase::id_type
RelationalDatabase::save(const oh5::File& f,
                         const QString& proposed_filename,
                         unsigned int filename_version) {
    QStringList columns, binds;
    columns.append("source_id");
    columns.append("unique_id");

    const MappingVector& special = mapper().specializations_on("files");
    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: get rid of this
        if (mapping.attribute == "file_id") 
            continue;
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const QString& column, columns) {
        binds.append(":" + column);
    }

    QSqlQuery qry(connection());
    QString qrystr("INSERT INTO files(" + columns.join(", ") +
                   ", proposed_filename, filename_version) "
                   "VALUES(" + binds.join(", ") +
                   ", :proposed_filename, :filename_version)");
    if (supports_returning())
        qrystr += " RETURNING id";
    qry.prepare(qrystr);

    if (not f.source())
        throw db_error("no Source associated with File");

    BOOST_FOREACH(const Mapping& mapping, special) {
        // XXX: do something about it, we can't continue adding here
        if (mapping.attribute == "path" || mapping.attribute == "file_id")
            continue;
        const QVariant& value = 
                f.root()->child_attribute(mapping.attribute)->value().to_qvariant();
        qry.bindValue(":" + mapping.column, value);
    }

    qry.bindValue(":path", f.path());
    qry.bindValue(":unique_id", file_hasher_->hash(f));
    qry.bindValue(":source_id", f.source()->db_id());
    qry.bindValue(":proposed_filename", proposed_filename);
    qry.bindValue(":filename_version", filename_version);

    if (not qry.exec())
        throw db_error(qry.lastError());
    if (supports_returning()) {
        qry.first();
        return qry.value(0);
    } else {
        return qry.lastInsertId();
    }
}

void
RelationalDatabase::load_source_centre(shared_ptr<oh5::SourceCentre> src) {
    QStringList wcl;
    QList<QVariant> binds;

    // originating_centre or country_code is required for org. sources
    if (src->originating_centre()) {
        wcl.append("originating_centre = ?");
        binds.append(src->originating_centre());
    }
    if (src->country_code()) {
        wcl.append("country_code = ?");
        binds.append(src->country_code());
    }
    if (src->db_id()) {
        wcl.append("sources.id = ?");
        binds.append(src->db_id());
    }

    QString qstr = QString("SELECT sources.id, node_id, country_code, ") +
                   QString("originating_centre, wmo_cccc ") +
                   QString("FROM source_centres ") +
                   QString("JOIN sources ON sources.id = source_centres.id ") +
                   QString("WHERE ") + wcl.join(" OR ");

    QSqlQuery qry(connection());
    qry.prepare(qstr);
    BOOST_FOREACH(const QVariant& bind, binds) {
        qry.addBindValue(bind);
    }

    if (not qry.exec()) 
        throw db_error(qry.lastError());
    if (qry.size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().toStdString());
    } else if (qry.size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().toStdString());
    }
    qry.first();

    src->db_id(qry.value(0).toLongLong());
    src->node_id(qry.value(1).toString());
    src->country_code(qry.value(2).toInt());
    src->originating_centre(qry.value(3).toInt());
    src->wmo_cccc(qry.value(4).toString());
}

void
RelationalDatabase::load_source_radar(shared_ptr<oh5::SourceRadar> src) {
    QStringList wcl;
    QList<QVariant> binds;

    // wmo_code or radar_site is required for radar sources
    if (src->wmo_code()) {
        wcl.append("wmo_code = ?");
        binds.append(src->wmo_code());
    }
    if (src->radar_site() != "") {
        wcl.append("radar_site = ?");
        binds.append(src->radar_site());
    }
    if (src->place() != "") {
        wcl.append("place = ?");
        binds.append(src->place());
    }

    QString qstr = QString("SELECT sources.id, node_id, centre_id, ") +
                   QString("wmo_code, radar_site, place ") +
                   QString("FROM source_radars ") +
                   QString("JOIN sources ON source_radars.id = sources.id ") +
                   QString("WHERE ") + wcl.join(" OR ");
    
    QSqlQuery qry(connection());
    qry.prepare(qstr);
    BOOST_FOREACH(const QVariant& bind, binds) {
        qry.addBindValue(bind);
    }

    if (not qry.exec()) 
        throw db_error(qry.lastError());
    if (qry.size() < 1) {
        throw lookup_error("no source found: " +
                           src->to_string().toStdString());
    } else if (qry.size() > 1) {
        throw lookup_error("multiple sources found: " +
                           src->to_string().toStdString());
    }
    qry.first();

    shared_ptr<oh5::SourceCentre> centre(new oh5::SourceCentre());
    centre->db_id(qry.value(2).toLongLong()); 
    load_source_centre(centre);

    src->centre(centre);
    src->db_id(qry.value(0).toLongLong());
    src->node_id(qry.value(1).toString());
    src->wmo_code(qry.value(3).toInt());
    src->radar_site(qry.value(4).toString());
    src->place(qry.value(5).toString());
}

shared_ptr<oh5::Source>
RelationalDatabase::do_load_source(const QString& srcstr) {

    shared_ptr<oh5::Source> src = oh5::Source::from_source_attribute(srcstr);
    shared_ptr<oh5::SourceRadar> radar;
    shared_ptr<oh5::SourceCentre> centre;

    if (radar = dynamic_pointer_cast<oh5::SourceRadar>(src)) {
        load_source_radar(radar);
        return radar;
    } else if (centre = dynamic_pointer_cast<oh5::SourceCentre>(src)) {
        load_source_centre(centre);
        return centre;
    } else {
        throw lookup_error(srcstr.toUtf8().constData());
    }

}

shared_ptr<ResultSet>
RelationalDatabase::query(const QString& query_str,
                          const BindMap& binds) {
    BRFC_ASSERT(connection().isOpen());
    QSqlQuery query(connection());
    query.prepare(query_str);
    BOOST_FOREACH(const BindMap::value_type& bind, binds) {
        query.bindValue(bind.first, bind.second.to_qvariant());
    }
    query.exec();
    return shared_ptr<ResultSet>(new RelationalResultSet(query, sql_));
}

void
RelationalDatabase::populate_mapper_and_specs() {
    mapper_->clear();
    specs_->clear();
    shared_ptr<ResultSet> r = query("SELECT id, name, converter, "
                                    "storage_table, storage_column, "
                                    "ignore_in_hash "
                                    "FROM attributes", BindMap());
    while (r->next()) {
        mapper_->add(Mapping(r->integer(0), // id
                             r->string(1),  // name
                             r->string(3),  // table
                             r->string(4))); // column
        specs_->add(oh5::AttributeSpec(r->string(1), // name
                                       r->string(2), // typename
                                       r->boolean(5))); // ignored in hash
    }
}

void
RelationalDatabase::do_remove_file(const QString& path) {
    QSqlQuery query(connection());
    query.prepare("DELETE FROM files WHERE path = :path");
    query.bindValue(":path", path);
    if (!query.exec())
        throw db_error(query.lastError());
}

} // namespace rdb
} // namespace brfc
