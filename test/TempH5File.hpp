#ifndef BRFC_TEMPORARY_H5_FILE_HPP
#define BRFC_TEMPORARY_H5_FILE_HPP

#include <brfc/hlhdf.hpp>
#include <brfc/smart_ptr.hpp>

#include <QtCore/QString>

class QDate;
class QString;
class QTime;

namespace brfc {

class Variant;

class TempH5File {
  public:
    TempH5File();

    /**
     * @brief construct with mandatory attributes present
     * @param object /what/object
     * @param date /what/date
     * @param time /what/time
     * @param source /what/source
     * @param version /what/version
     *
     * this is the minimal "correct" file, given that parameters are
     * correctly formed.
     */
    static auto_ptr<TempH5File>
        minimal(const QString& object,
                const QDate& date,
                const QTime& time,
                const QString& source,
                const QString& version=QString::fromUtf8("H5rad 2.0"));

    ~TempH5File();
    void add_group(const char* path);
    void add_attribute(const char* path, const Variant& value);
    void write();
    QString filename() const;

  private:
    shared_ptr<HL_NodeList> nodes_;
    shared_ptr<char> filename_;
    int fd_;
};

} // namespace brfc

#endif // BRFC_TEMPORARY_H5_FILE_HPP
