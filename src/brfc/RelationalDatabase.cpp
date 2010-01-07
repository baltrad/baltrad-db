#include <brfc/RelationalDatabase.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/File.hpp>
#include <brfc/Source.hpp>
#include <brfc/DataObject.hpp>
#include <brfc/Attribute.hpp>
#include <brfc/AttributeMapper.hpp>
#include <brfc/ResultSet.hpp>
#include <brfc/Query.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/AttrReplace.hpp>
#include <brfc/expr/Compiler.hpp>
#include <brfc/expr/Expression.hpp>
#include <brfc/expr/Select.hpp>
#include <brfc/expr/Table.hpp>

#include <boost/foreach.hpp>
#include <boost/variant.hpp>
#include <boost/algorithm/string/join.hpp>

#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include <map>

namespace brfc {

QString
RelationalDatabase::qt_engine(const QString& /*engine*/) const {
    return "QPSQL";
}

RelationalDatabase::RelationalDatabase(const std::string& dsn_)
        : sql() {
    QUrl dsn(dsn_.c_str());
    sql.reset(new ::QSqlDatabase(::QSqlDatabase::addDatabase(qt_engine(dsn.scheme()), "brfc")));
    sql->setHostName(dsn.host());
    sql->setUserName(dsn.userName());
    sql->setPassword(dsn.password());
    QString database = dsn.path();
    if (database.startsWith("/")) {
        database.remove(0, 1); // remove slash
    }
    sql->setDatabaseName(database);
    if (!sql->open())
        throw db_error(sql->lastError().text().toStdString());
}

RelationalDatabase::~RelationalDatabase() {
    sql->close();
    sql.reset(0);
    ::QSqlDatabase::removeDatabase("brfc");
}

void
RelationalDatabase::do_begin() {
    sql->transaction();
}

void
RelationalDatabase::do_rollback() {
    sql->rollback();
}

void
RelationalDatabase::do_commit() {
    sql->commit();
}

void
RelationalDatabase::do_save_file(const File& file,
                                 const AttributeMapper& mapper) {
    save_recurse(file, mapper);
}

ResultSet
RelationalDatabase::do_query(const Query& query) {
    expr::SelectPtr select = expr::Select::create();

    BOOST_FOREACH(expr::AttributePtr expr, query.fetch()) {
        select->what(expr);
    }

    select->where(query.filter());
    select->distinct(query.distinct());

    expr::TablePtr data_objects_t = expr::Table::create("data_objects");
    expr::TablePtr files_t = expr::Table::create("files");

    expr::AttrReplace::replace(select, query.mapper());
    expr::Compiler compiler;
    compiler.compile(*select);

    return this->query(QString::fromStdString(compiler.compiled()),
                       compiler.binds());
}

namespace {

class Insert {
  public:
    typedef std::map<QString, QVariant> BindMap;

    Insert(const std::string& table)
          : table_(table.c_str()) {
    }
        
    void bind(const QString& key, const QVariant& value) {
        binds_[key] = value;
    }

    void returning(const std::string& col) {
        returning_.push_back(col.c_str());
    }
  
    QSqlQuery query(const ::QSqlDatabase& db) const {
        QStringList cols, vals;
        BOOST_FOREACH(const BindMap::value_type& bind, binds_) {
            cols.push_back(bind.first);
            vals.push_back(":" + bind.first);
        }

        QStringList query_str;
        query_str << "INSERT INTO " << table_
                  << "(" << cols.join(", ") << ") "
                  << "VALUES (" << vals.join(", ") << ")";
        if (!returning_.empty())
            query_str << " RETURNING " << returning_.join(", ");

        QSqlQuery q(db);
        q.prepare(query_str.join(""));

        BOOST_FOREACH(const BindMap::value_type& bind, binds_) {
            q.bindValue(":" + bind.first, bind.second);
        }

        return q;
    }

    std::string table() const {
        return table_.toStdString();
    }

  private:
    QString table_;
    QStringList returning_;
    BindMap binds_;
};

void
extend_for_specializations(Insert& insert,
                           const DataObject& dobj,
                           const AttributeMapper& mapper) {
    MappingVector v = mapper.specializations_on(insert.table());
    BOOST_FOREACH(const Mapping& m, v) {
        try {
            insert.bind(m.column.c_str(), dobj.attribute(m.attribute).value());
        } catch (const brfc::lookup_error& e) {
            insert.bind(m.column.c_str(), QVariant());
        }
    }
}

} // namespace anonymous


RelationalDatabase::id_type
RelationalDatabase::save(const File& f, const AttributeMapper& mapper) {
    Insert stmt("files");
    id_type source_id = query_id(f.source());
    if (source_id.isNull()) {
        throw db_error("could not db-lookup source: " +
                       f.root().attribute("source").value().toString().toStdString());
    }
    stmt.bind("source_id", source_id);
    stmt.returning("id");
    
    extend_for_specializations(stmt, f.root(), mapper);

    QSqlQuery query = stmt.query(*sql);

    if (!query.exec())
        throw db_error(query.lastError());
    query.first();
    return query.value(0);
}

void
RelationalDatabase::save_recurse(const File& f,
                                 const AttributeMapper& mapper) {
    typedef std::map<const DataObject*, id_type> IdCache;

    id_type file_id = save(f, mapper);
    
    IdCache id_cache;
    const id_type* parent_id;
    id_type null;
    BOOST_FOREACH(const DataObject& dobj, f.root()) {
        if (dobj.parent()) {
            const DataObject* parent = dobj.parent();
            parent_id = &id_cache[parent];
        } else {
            parent_id = &null;
        }
        id_type dobj_id = save_recurse(dobj, file_id, *parent_id, mapper);
        id_cache[&dobj] = dobj_id;
    }
}

RelationalDatabase::id_type
RelationalDatabase::save(const DataObject& d,
                         const id_type& file_id,
                         const id_type& parent_id,
                         const AttributeMapper& mapper) {
    Insert stmt("data_objects");
    stmt.bind("parent_id", parent_id);
    stmt.bind("file_id", file_id);
    stmt.bind("name", d.name().c_str());
    stmt.returning("id");

    extend_for_specializations(stmt, d, mapper);

    QSqlQuery query = stmt.query(*sql);

    if (!query.exec())
        throw db_error(query.lastError());
    query.first();
    return query.value(0);
}

RelationalDatabase::id_type
RelationalDatabase::save_recurse(const DataObject& dobj,
                                 const id_type& file_id,
                                 const id_type& parent_id,
                                 const AttributeMapper& mapper) {
    id_type dobj_id = save(dobj, file_id, parent_id, mapper);
    BOOST_FOREACH(const Attribute& attr, dobj.attributes()) {
        save(attr, dobj_id, mapper);
    }
    return dobj_id;
}

void
RelationalDatabase::save(const Attribute& attr,
                         const id_type& dobj_id,
                         const AttributeMapper& mapper) {
    if (mapper.is_specialized(attr.name()))
        return;

    Mapping tc = mapper.mapping(attr.name());

    Insert stmt(tc.table);
    stmt.bind("data_object_id", dobj_id);
    stmt.bind("attribute_id", mapper.spec(attr.name()).id);
    stmt.bind("value", attr.value());

    QSqlQuery query = stmt.query(*sql);

    if (!query.exec())
        throw db_error(query.lastError());
}

RelationalDatabase::id_type
RelationalDatabase::query_id(const File& f) {
    QSqlQuery query(*sql);
    query.prepare("SELECT id FROM files "
                  "WHERE path = :path ");
    query.bindValue(":path", f.path().c_str());
    query.exec();
    query.first();
    return query.value(0);
}

RelationalDatabase::id_type
RelationalDatabase::query_id(const DataObject& dobj) {
    id_type parent_id;

    if (dobj.parent()) {
        parent_id = query_id(*dobj.parent());
    }

    id_type file_id = query_id(*dobj.file());
    std::string op = parent_id.isNull() ? "IS NULL" : "= :parent_id";
    const std::string& name = dobj.name();
    QSqlQuery query(*sql);
    query.prepare(("SELECT id FROM data_objects "
                   "WHERE parent_id " + op + " "
                      "AND file_id = :file_id "
                      "AND name = :name").c_str());
    query.bindValue(":parent_id", parent_id);
    query.bindValue(":file_id", file_id);
    query.bindValue(":name", name.c_str());
    query.exec();
    query.first();
    return query.value(0);
}

RelationalDatabase::id_type
RelationalDatabase::query_id(const Source& src) {
    QString query_str("SELECT id FROM sources WHERE ");
    QVariant bindvar;
    
    // singe-site data
    if (src.wmo_code().isValid()) {
        query_str += "wmo_code = ?";
        bindvar = src.wmo_code();
    } else if (src.radar_site().isValid()) {
        query_str += "radar_site = ?";
        bindvar = src.radar_site();
    // composites
    } else if (src.originating_centre().isValid()) {
        query_str += "originating_centre = ?";
        bindvar = src.radar_site();
    } else if (src.country_code().isValid()) {
        query_str += "country_code = ?";
        bindvar = src.country_code();
    }

    QSqlQuery query(*sql);
    query.prepare(query_str);
    query.bindValue(0, bindvar);
    query.exec();
    query.first();
    return query.value(0);
}

ResultSet
RelationalDatabase::query(const QString& query_str,
                          const BindMap& binds) const {
    BRFC_ASSERT(sql->isOpen());
    QSqlQuery query(*sql);
    query.prepare(query_str);
    BOOST_FOREACH(const BindMap::value_type& bind, binds) {
        query.bindValue(bind.first, bind.second);
    }
    query.exec();
    return ResultSet(query);
}

void
RelationalDatabase::do_populate_attribute_mapper(AttributeMapper& mapper) {
    ResultSet r = query("SELECT id, name, converter, "
                               "storage_table, storage_column "
                        "FROM attributes", BindMap());
    while (r.next()) {
        mapper.add_spec(r.integer(0), // id
                        r.string(1),  // name
                        r.string(2),  // converter
                        r.string(3),  // table
                        r.string(4)); // column
    }
}

void
RelationalDatabase::do_remove_file(const char* path) {
    QSqlQuery query(*sql);
    query.prepare("DELETE FROM files WHERE path = :path");
    query.bindValue(":path", path);
    if (!query.exec())
        throw db_error(query.lastError());
}

void
RelationalDatabase::do_clean() {
    BRFC_ASSERT(sql->isOpen());
    QSqlQuery query(*sql);
    query.exec("DELETE FROM files");
}

}
