#ifndef BRFC_TEMPORARY_H5_FILE_HPP
#define BRFC_TEMPORARY_H5_FILE_HPP

#include <brfc/hlhdf.hpp>
#include <brfc/smart_ptr.hpp>
#include <brfc/Converter.hpp>

class QVariant;
class QDate;
class QTime;

namespace brfc {

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
        minimal(const std::string& object,
                const QDate& date,
                const QTime& time,
                const std::string& source,
                const std::string& version="H5rad 2.0");

    ~TempH5File();
    void add_group(const char* path);
    void add_attribute(const char* path, const QVariant& value);
    void write();
    const char* filename() const;

  private:
    HL_Data convert(const QVariant& value);
    
    shared_ptr<HL_NodeList> nodes_;
    shared_ptr<char> filename_;
    int fd_;
};

} // namespace brfc

#endif // BRFC_TEMPORARY_H5_FILE_HPP
