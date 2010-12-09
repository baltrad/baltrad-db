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
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

#include <brfc/fuse/DirFactory.hpp>

#include <brfc/assert.hpp>
#include <brfc/Variant.hpp>

#include <brfc/db/AttributeQuery.hpp>
#include <brfc/db/AttributeResult.hpp>
#include <brfc/db/Database.hpp>

#include <brfc/expr/Attribute.hpp>
#include <brfc/expr/BinaryOperator.hpp>
#include <brfc/expr/ExpressionFactory.hpp>
#include <brfc/expr/Literal.hpp>

namespace brfc {
namespace fuse {

DirFactory::DirFactory(db::Database* db,
                       const expr::Attribute& attr,
                       const EntryFactory* cfactory)
        : db_(db)
        , attr_(static_pointer_cast<expr::Attribute>(attr.clone()))
        , cfactory_(cfactory)
        , filter_() 
        , entries_() {
    BRFC_ASSERT(db_);
    BRFC_ASSERT(cfactory_);
}

DirFactory::DirFactory(const DirFactory& other)
        : db_(other.db_)
        , attr_(other.attr_)
        , cfactory_(other.cfactory_)
        , filter_(other.filter_)
        , entries_(other.entries_) {

}

DirFactory::~DirFactory() {

}

DirFactory*
DirFactory::do_clone() const {
    return new DirFactory(*this);
}

void
DirFactory::do_filter(const expr::Expression& expr) {
    expr::ExpressionPtr c = expr.clone();
    filter_.swap(c);
}

void
DirFactory::do_update() {
    invalidate_all();

    db::AttributeQuery qry;
    qry.fetch(*attr_);
    if (filter_)
        qry.filter(*filter_);
    
    shared_ptr<db::AttributeResult> result = database().execute(qry);

    EntryByName_t& es = entries_.get<by_name>();
    EntryByName_t::iterator iter;

    while (result->next()) {
        const Variant& value = result->value_at(0);
        iter = es.find(value.to_string());
        if (iter != es.end()) {
            es.modify(iter, set_valid(true));
        } else {
            es.insert(create_entry(value));
        }
    }

    remove_invalid();
}

std::vector<std::string>
DirFactory::do_entry_names() const {
    std::vector<std::string> names;
    names.reserve(entries_.size());
    for (EntrySet_t::const_iterator iter = entries_.begin();
         iter != entries_.end();
         ++iter) {
        names.push_back((*iter)->name());
    }

    return names;
}

Entry*
DirFactory::do_entry_by_name(const std::string& name) const {
    const EntryByName_t& es = entries_.get<by_name>();
    EntryByName_t::const_iterator it = es.find(name);
    return it != es.end() ? (*it).get() : 0;
}

void
DirFactory::invalidate_all() {
    EntrySet_t::iterator iter;
    for (iter = entries_.begin(); iter != entries_.end(); ++iter) {
        entries_.modify(iter, set_valid(false));
    }
}

void
DirFactory::remove_invalid() {
    EntryByValid_t& es = entries_.get<by_valid>();
    es.erase(es.lower_bound(false), es.upper_bound(false));
}


shared_ptr<DirEntry>
DirFactory::create_entry(const Variant& value) {
    static expr::ExpressionFactory xpr;
    shared_ptr<DirEntry> entry = make_shared<DirEntry>(value.to_string(),
                                                       *cfactory_);
        
    expr::ExpressionPtr cfilter = attr_->eq(expr::Literal((value)));
    if (filter_)
        cfilter = xpr.and_(*filter_, *cfilter);
    entry->child_factory().filter(*cfilter);
    return entry;
}

} // namespace fuse
} // namespace brfc
