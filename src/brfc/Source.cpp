#include <brfc/Source.hpp>

#include <brfc/assert.hpp>
#include <brfc/exceptions.hpp>

#include <boost/foreach.hpp>

#include <vector>

#include <QtCore/QStringList>

namespace brfc {

Source::Source()
        : map_() {
    init();
}
        
Source::Source(const QString& source)
        : map_() {
    init();
    parse_source(source);
}

void
Source::init() {
    map_["WMO"] = QVariant();
    map_["RAD"] = QVariant();
    map_["ORG"] = QVariant();
    map_["PLC"] = QVariant();
    map_["CTY"] = QVariant();
    map_["CMT"] = QVariant();
}

void Source::parse_source(const QString& source) {
    QStringList elems = source.split(",");
    try {
        BRFC_ASSERT(elems.size() != 0);
        BOOST_FOREACH(const QString& elem, elems) {
            QStringList kv = elem.split(":");
            BRFC_ASSERT(kv.size() == 2);
            BRFC_ASSERT(kv[0].length() > 0);
            BRFC_ASSERT(kv[1].length() > 0);
            if (map_.find(kv.at(0)) == map_.end()) {
                throw value_error(source.toStdString());
            }
            map_[kv.at(0)] = kv.at(1);
        }
    } catch (const assertion_error& e) {
        throw value_error(e.what());
    }
}


QVariant
Source::get_opt(const QString& key) const {
    ElementMap::const_iterator i = map_.find(key);
    BRFC_ASSERT(i != map_.end());
    return i->second;
}

} // namespace brfc
