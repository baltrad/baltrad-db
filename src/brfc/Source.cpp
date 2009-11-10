#include <brfc/Source.hpp>

#include <brfc/assert.hpp>

#include <boost/foreach.hpp>

#include <stdexcept>
#include <vector>

#include <QtCore/QStringList>

namespace brfc {

Source::Source()
        : map_() {
}
        
Source::Source(const QString& source)
        : map_() {
    parse_source(source);
}

void Source::parse_source(const QString& source) {
    QStringList elems = source.split(",");
    BRFC_ASSERT(elems.size() != 0);
    BOOST_FOREACH(const QString& elem, elems) {
        QStringList kv = elem.split(":");
        BRFC_ASSERT(kv.size() == 2);
        BRFC_ASSERT(kv[0].length() > 0);
        BRFC_ASSERT(kv[1].length() > 0);
        map_[kv.at(0)] = kv.at(1);
    }
}


QVariant
Source::get_opt(const QString& key) const {
    ElementMap::const_iterator i = map_.find(key);
    return i == map_.end() ? QVariant() : i->second;
}

} // namespace brfc
