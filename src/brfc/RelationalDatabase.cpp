#include <brfc/RelationalDatabase.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/File.hpp>
#include <brfc/Source.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/AttributeSpecs.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/RelationalResultSet.hpp>
#include <brfc/Query.hpp>
#include <brfc/Variant.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/AttrReplace.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Select.hpp>

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

namespace {

class DataObjectSaver {
  public:
    DataObjectSaver(RelationalDatabase* rdb)
            : rdb_(rdb)
            , qry_(rdb->connection())
            , special_(rdb->mapper().specializations_on("data_objects")) {
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
        QString qrystr("INSERT INTO data_objects(" + columns.join(", ") +
                       ") VALUES(" + binds.join(", ") + ")");
        if (rdb->supports_returning())
            qrystr += " RETURNING id";
        if (not qry_.prepare(qrystr))
            throw db_error(qry_.lastError());
    }
    
    void operator()(const DataObject& dobj) {
        bind_plain(dobj);
        bind_specializations(dobj);

        if (not qry_.exec())
            throw db_error(qry_.lastError());
        dobj.db_id(last_id());
    }

  private:
    void bind_plain(const DataObject& dobj) {
        QVariant file_id, parent_id;
        if (dobj.parent())
            parent_id = dobj.parent()->db_id();
        if (dobj.file())
            file_id = dobj.file()->db_id();

        qry_.bindValue(":parent_id", parent_id);
        qry_.bindValue(":file_id", file_id);
        qry_.bindValue(":name", dobj.name());
    }

    void bind_specializations(const DataObject& dobj) {
        BOOST_FOREACH(const Mapping& mapping, special_) {
            QVariant val;
            try {
                val = dobj.attribute(mapping.attribute).value().to_qvariant();
            } catch (const brfc::lookup_error& e) {
                // value is null
            }
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

    void operator()(const Attribute& attr) {
        // ignore specialized attributes
        if (mapper_->is_specialized(attr.name()))
            return;

        const Mapping& mapping = mapper_->mapping(attr.name());

        QSqlQuery& qry = query_by_table(mapping.table);
        QVariant dobj_id;
        if (attr.data_object())
            dobj_id = attr.data_object()->db_id();
        qry.bindValue("data_object_id", dobj_id);
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
                        "(data_object_id, attribute_id, value) " +
                        "VALUES(:data_object_id, :attribute_id, :value)");
            QueryMap::value_type val(table, qry);
            iter = queries_.insert(val).first;
        }
        return iter->second;
    }

    RelationalDatabase* rdb_;
    const AttributeMapper* mapper_;
    QueryMap queries_;
};

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
        , specs_(new AttributeSpecs())
        , dialect_()
        , supports_returning_(false) {
    init_qapp();
    QUrl dsn(dsn_);
    QString name = QString("brfc-") + QString::number(connection_count_++);
    sql_ = ::QSqlDatabase::addDatabase(qt_engine(dsn.scheme()), name);
    sql_.setHostName(dsn.host());
    sql_.setUserName(dsn.userName());
    sql_.setPassword(dsn.password());
    QString database = dsn.path();
    if (database.startsWith("/")) {
        database.remove(0, 1); // remove slash
    }
    sql_.setDatabaseName(database);
    if (!sql_.open())
        throw db_error(sql_.lastError());
    dialect_ = dsn.scheme();
    if (dialect_ == "postgresql")
        supports_returning_ = true;
    populate_mapper_and_specs();
    if (instance_count_ == 0) {
        init_qapp();
    }
    ++instance_count_;
}

void
RelationalDatabase::init_qapp() {
    qapp_ = new QCoreApplication(argc_, const_cast<char**>(argv_));
}

unsigned int RelationalDatabase::connection_count_ = 0;
unsigned int RelationalDatabase::instance_count_ = 0;
int RelationalDatabase::argc_ = 1;
const char* RelationalDatabase::argv_[] = {"brfc"};
QCoreApplication* RelationalDatabase::qapp_ = 0;

RelationalDatabase::~RelationalDatabase() {
    --instance_count_;
    sql_.close();
    ::QSqlDatabase::removeDatabase(sql_.connectionName());
    if (instance_count_ == 0) {
        delete qapp_;
        qapp_ = 0;
    }
}

const AttributeSpecs&
RelationalDatabase::specs() const {
    return *specs_;
}

const AttributeMapper&
RelationalDatabase::mapper() const {
    return *mapper_;
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
RelationalDatabase::do_has_file(const File& file) {
    QSqlQuery query(connection());
    query.prepare("SELECT true FROM files WHERE unique_id = :unique_id");
    query.bindValue(":unique_id", file.unique_identifier());
    if (!query.exec())
        throw db_error(query.lastError());
    return query.next(); // got a result row
}

long long
RelationalDatabase::do_save_file(const QString& path, const File& file) {
    id_type file_id = save(path, file);
    file.db_id(file_id.toLongLong());

    DataObjectSaver save_data_object(this);
    AttributeSaver save_attribute(this);
    
    BOOST_FOREACH(const DataObject& dobj, file.root()) {
        save_data_object(dobj);
        BOOST_FOREACH(shared_ptr<Attribute> attr, dobj.attributes()) {
            save_attribute(*attr);
        }
    }

    return file_id.toLongLong();
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
RelationalDatabase::save(const QString& path, const File& f) {
    QStringList columns, binds;
    columns.append("source_id");
    columns.append("unique_id");

    const MappingVector& special = mapper().specializations_on("files");
    BOOST_FOREACH(const Mapping& mapping, special) {
        columns.append(mapping.column);
    }
    BOOST_FOREACH(const QString& column, columns) {
        binds.append(":" + column);
    }

    QSqlQuery qry(connection());
    QString qrystr("INSERT INTO files(" + columns.join(", ") +
                   ") VALUES(" + binds.join(", ") + ")");
    if (supports_returning())
        qrystr += " RETURNING id";
    qry.prepare(qrystr);

    if (not f.source())
        throw db_error("no Source associated with File");

    BOOST_FOREACH(const Mapping& mapping, special) {
        if (mapping.attribute == "path")
            continue;
        const QVariant& value = 
                f.root().attribute(mapping.attribute).value().to_qvariant();
        qry.bindValue(":" + mapping.column, value);
    }

    qry.bindValue(":path", path);
    qry.bindValue(":unique_id", f.unique_identifier());
    qry.bindValue(":source_id", f.source()->db_id());

    if (not qry.exec())
        throw db_error(qry.lastError());
    if (supports_returning()) {
        qry.first();
        return qry.value(0);
    } else {
        return qry.lastInsertId();
    }
}

shared_ptr<Source>
RelationalDatabase::do_load_source(const QString& srcstr) {
    QStringList wcl;
    QList<QVariant> binds;

    Source src = Source::from_source_attribute(srcstr);

    // wmo_code or radar_site is required for radar sources
    if (src.wmo_code()) {
        wcl.append("wmo_code = ?");
        binds.append(src.wmo_code());
    } else if (src.radar_site() != "") {
        wcl.append("radar_site = ?");
        binds.append(src.radar_site());
    // originating_centre or country_code is required for org. sources
    } else if (src.originating_centre()) {
        wcl.append("originating_centre = ?");
        binds.append(src.originating_centre());
    } else if (src.country_code()) {
        wcl.append("country_code = ?");
        binds.append(src.country_code());
    }

    QString qstr = QString("SELECT id, country_code, wmo_code, ") +
                   QString("radar_site, originating_centre, node_id, ") +
                   QString("place FROM sources WHERE ") + wcl.join(" OR ");

    QSqlQuery query(connection());
    query.prepare(qstr);
    BOOST_FOREACH(const QVariant& bind, binds) {
        query.addBindValue(bind);
    }

    if (not query.exec()) 
        throw db_error(query.lastError());
    if (query.size() < 1) {
        throw lookup_error("no source found");
    } else if (query.size() > 1) {
        throw lookup_error("multiple sources found");
    }
    query.first();

    shared_ptr<Source> src_p(new Source());

    src_p->db_id(query.value(0).toLongLong());
    src_p->country_code(query.value(1).toInt());
    src_p->wmo_code(query.value(2).toInt());
    src_p->radar_site(query.value(3).toString());
    src_p->originating_centre(query.value(4).toInt());
//    src_p->node_id(query.value(5).toString());
    src_p->place(query.value(6).toString());

    return src_p;
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
    return shared_ptr<ResultSet>(new RelationalResultSet(query));
}

void
RelationalDatabase::populate_mapper_and_specs() {
    mapper_->clear();
    specs_->clear();
    shared_ptr<ResultSet> r = query("SELECT id, name, converter, "
                                  "storage_table, storage_column "
                                  "FROM attributes", BindMap());
    while (r->next()) {
        mapper_->add(Mapping(r->integer(0), // id
                             r->string(1),  // name
                             r->string(3),  // table
                             r->string(4))); // column
        specs_->add(r->string(1), r->string(2)); // name, typename
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

void
RelationalDatabase::do_clean() {
    BRFC_ASSERT(connection().isOpen());
    QSqlQuery query(connection());
    query.exec("DELETE FROM files");
}

}
