#include <brfc/Source.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>
#include <brfc/SourceCentre.hpp>
#include <brfc/SourceRadar.hpp>

#include <boost/foreach.hpp>

#include <QtCore/QStringList>
#include <QtCore/QMap>

namespace brfc {

namespace {

typedef QMap<QString, QString> ElementMap;
    
ElementMap
parse_source(const QString& source) {
    ElementMap map;
    QStringList elems = source.split(",");
    try {
        BRFC_ASSERT(elems.size() != 0);
        BOOST_FOREACH(const QString& elem, elems) {
            QStringList kv = elem.split(":");
            BRFC_ASSERT(kv.size() == 2);
            BRFC_ASSERT(kv[0].length() > 0);
            BRFC_ASSERT(kv[1].length() > 0);
            map[kv.at(0)] = kv.at(1);
        }
    } catch (const assertion_error& e) {
        throw value_error(e.what());
    }
    return map;
}

} // namespace anonymous

shared_ptr<Source>
Source::from_source_attribute(const QString& source) {
    const ElementMap& elems = parse_source(source);
    if (elems.contains("WMO") or
        elems.contains("RAD") or
        elems.contains("PLC")) {
        
        shared_ptr<SourceRadar> src(new SourceRadar());
        src->wmo_code(elems.value("WMO", "0").toInt());
        src->radar_site(elems.value("RAD", ""));
        src->place(elems.value("PLC", ""));
        return src;
    } else if (elems.contains("CTY") or elems.contains("ORG")) {
        shared_ptr<SourceCentre> src(new SourceCentre());
        src->country_code(elems.value("CTY", "0").toInt());
        src->originating_centre(elems.value("ORG", "0").toInt());
        return src;
    } else {
        throw value_error("no fields in source to determine type");
    }
}

} // namespace brfc
