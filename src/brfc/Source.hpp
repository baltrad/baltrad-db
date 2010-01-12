#ifndef BRFC_SOURCE_H
#define BRFC_SOURCE_H

#include <map>
#include <QtCore/QVariant>

namespace brfc {

class Source {
  public:
    Source();
    /**
     * @brief construct from string
     *
     * valid elements are [WMO,RAD,ORG,PLC,CTY,CMT]
     * string should consist of element-value pairs, where
     * element and value are separated by a colon.
     * pairs are separated by comma.
     */
    Source(const QString& source);

    /**
     * @brief combined WMO block and station number
     */
    QVariant wmo_code() const {
        const QVariant& val = get_opt("WMO");
        return val.isValid() ? val.toInt() : val;
    }

    /**
     * @brief radar site as indexed in the OPERA database
     */
    QVariant radar_site() const { return get_opt("RAD"); }

    /**
     * @brief originating centre
     */
    QVariant originating_centre() const {
        const QVariant& val = get_opt("ORG");
        return val.isValid() ? val.toInt() : val;
    }

    /**
     * @brief place according to ODIM 2.0 spec, table 9
     */
    QVariant place() const { return get_opt("PLC"); }

    /**
     * @brief country code according to BUFR tables
     */
    QVariant country_code() const {
        const QVariant& val = get_opt("CTY");
        return val.isValid() ? val.toInt() : val;
    }

    /**
     * @brief comment
     */
    QVariant comment() const { return get_opt("CMT"); }

    /**
     * @brief does this source identify a radar site
     */
    bool identifies_radar() const;
    
  private:
    typedef std::map<QString, QVariant> ElementMap;

    void init();

    void parse_source(const QString& source);

    QVariant get_opt(const QString& key) const;

    ElementMap map_;
};

}

#endif // BRFC_SOURCE_H
