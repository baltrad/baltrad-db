#ifndef BRFC_SOURCE_H
#define BRFC_SOURCE_H

#include <string>

class QString;

namespace brfc {

class Source {
  public:
    Source();

    /**
     * @brief construct from ODIM_H5 'what/source' metadata attribute
     *
     * valid elements are [WMO,RAD,ORG,PLC,CTY,CMT]
     * string should consist of element-value pairs, where
     * element and value are separated by a colon.
     * pairs are separated by comma.
     */
    static Source from_source_attribute(const QString& source);

    /**
     * @brief combined WMO block and station number
     */
    int wmo_code() const { return wmo_code_; }

    void wmo_code(int value) {
        wmo_code_ = value;
    }

    /**
     * @brief radar site as indexed in the OPERA database
     */
    const std::string& radar_site() const { return radar_site_; }

    void radar_site(const char* value) {
        radar_site_ = value;
    }

    /**
     * @brief originating centre
     */
    int originating_centre() const { return originating_centre_; }
    
    void originating_centre(int value) {
        originating_centre_ = value;
    }

    /**
     * @brief place according to ODIM 2.0 spec, table 9
     */
    const std::string& place() const { return place_; }

    void place(const char* value) {
        place_ = value;
    }

    /**
     * @brief country code according to BUFR tables
     */
    int country_code() const { return country_code_; }

    void country_code(int value) {
        country_code_ = value;
    }

    /**
     * @brief comment
     */
    const std::string& comment() const { return comment_; }

    void comment(const char* value) {
        comment_ = value;
    }

  private:
    long long db_id_;
    int wmo_code_;
    std::string radar_site_;
    int originating_centre_;
    std::string place_;
    int country_code_;
    std::string comment_;
};

}

#endif // BRFC_SOURCE_H
