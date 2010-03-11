#ifndef BRFC_OH5_SOURCE_H
#define BRFC_OH5_SOURCE_H

#include <brfc/smart_ptr.hpp>

#include <QtCore/QString>

namespace brfc {
namespace oh5 {

class Source {
  public:
    explicit Source(long long db_id=0, const QString& node_id="")
             : db_id_(db_id)
             , node_id_(node_id) {
    }

    virtual ~Source() { }

    /**
     * @brief construct from ODIM_H5 '/what/source' metadata attribute
     *
     * @param source contents of '/what/source'
     * @throw value_error when source contains invalid fields or not
     *        correctly formed
     *
     * valid elements are [WMO,RAD,ORG,PLC,CTY,CMT]
     * string should consist of element-value pairs, where
     * element and value are separated by a colon.
     * pairs are separated by comma.
     *
     */
    static shared_ptr<Source> from_source_attribute(const QString& source);

    long long db_id() const {
        return db_id_;
    }

    void db_id(long long db_id) {
        db_id_ = db_id;
    }

    QString node_id() const { return node_id_; }

    void node_id(const QString& value) {
        node_id_ = value;
    }

    virtual QString wmo_cccc() const = 0;

  private:
    long long db_id_;
    QString node_id_;
};

} // namespace oh5
} // namespace brfc

#endif // BRFC_OH5_SOURCE_H