#ifndef BRFC_OH5_SOURCE_CENTRE_HPP
#define BRFC_OH5_SOURCE_CENTRE_HPP

#include <brfc/oh5/Source.hpp>

namespace brfc {
namespace oh5 {

class SourceCentre : public Source {
  public:
    
    SourceCentre()
            : Source()
            , originating_centre_(0)
            , country_code_(0)
            , wmo_cccc_() {

    }

    /**
     * @brief originating centre
     */
    int originating_centre() const { return originating_centre_; }
    
    void originating_centre(int value) {
        originating_centre_ = value;
    }

    /**
     * @brief country code according to BUFR tables
     */
    int country_code() const { return country_code_; }

    void country_code(int value) {
        country_code_ = value;
    }

    /**
     * @brief WMO CCCC from Volume C1
     */
    virtual QString wmo_cccc() const { return wmo_cccc_; }

    void wmo_cccc(const QString& value) {
        wmo_cccc_ = value;
    }

    virtual QString to_string() const;

  private:
    int originating_centre_;
    int country_code_;
    QString wmo_cccc_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_CENTRE_HPP
