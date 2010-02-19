#include <brfc/Source.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <boost/foreach.hpp>

#include <map>

#include <QtCore/QStringList>

namespace brfc {

Source::Source()
        : db_id_(0)
        , wmo_code_(0)
        , radar_site_()
        , originating_centre_(0)
        , place_()
        , country_code_(0)
        , node_id_() {
}

namespace {

typedef std::map<QString, QString> ElementMap;
    
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

Source
Source::from_source_attribute(const QString& source) {
    const ElementMap& elements = parse_source(source);
    Source src;
    BOOST_FOREACH(const ElementMap::value_type& entry, elements) {
        if (entry.first == "WMO") {
            src.wmo_code(entry.second.toInt());
        } else if (entry.first == "RAD") {
            src.radar_site(entry.second);
        } else if (entry.first == "ORG") {
            src.originating_centre(entry.second.toInt());
        } else if (entry.first == "PLC") {
            src.place(entry.second);
        } else if (entry.first == "CTY") {
            src.country_code(entry.second.toInt());
        } else if (entry.first == "CMT") {
            // ignore, this is not relevant
        } else {
            QString err = QString("invalid field in source: ") + source;
            throw value_error(err.toUtf8().constData());
        }
    }
    return src;
}

} // namespace brfc
