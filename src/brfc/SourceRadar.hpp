#ifndef BRFC_SOURCE_RADAR_HPP
#define BRFC_SOURCE_RADAR_HPP

#include <brfc/Source.hpp>
#include <brfc/SourceCentre.hpp>
#include <brfc/assert.hpp>

namespace brfc {

class SourceRadar : public Source {
  public:
    SourceRadar()
            : wmo_code_(0)
            , radar_site_("")
            , place_("")
            , centre_() {
    }

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
    const QString& radar_site() const { return radar_site_; }

    void radar_site(const QString& value) {
        radar_site_ = value;
    }

    /**
     * @brief place according to ODIM 2.0 spec, table 9
     */
    const QString& place() const { return place_; }

    void place(const QString& value) {
        place_ = value;
    }

    shared_ptr<SourceCentre> centre() const { return centre_; }

    void centre(shared_ptr<SourceCentre> value) {
        centre_= value;
    }

    virtual QString wmo_cccc() const {
        BRFC_ASSERT(centre_);
        return centre_->wmo_cccc();
    }
   
  private:
    int wmo_code_;
    QString radar_site_;
    QString place_;
    shared_ptr<SourceCentre> centre_;
};

} // namespace brfc

#endif // BRFC_SOURCE_RADAR_HPP
