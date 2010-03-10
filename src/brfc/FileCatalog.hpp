#ifndef BRFC_FILE_CATALOGER_H
#define BRFC_FILE_CATALOGER_H

#include <brfc/smart_ptr.hpp>
#include <brfc/FileNamer.hpp>

#include <QtCore/QString>
#include <QtCore/QDir>

/**
 * @brief main namespace
 */
namespace brfc {
    
class Database;
class Query;

namespace oh5 {
    class AttributeSpecs;
    class File;
}

/**
 * @defgroup exposed_in_binds Exposed in bindings
 */

/**
 * @brief indexes and stores ODIM_H5 File instances
 * @ingroup exposed_in_binds
 */
class FileCatalog {
  public:
    /**
     * @brief constructor
     * @param dsn database connection string (using URL-like syntax)
     * @param storage absolute path to storage root
     *
     * @throw db_error if dsn is invalid or DB could not be opened
     * @throw fs_error if storage path does not exist
     *
     * on creating a FileCatalog instance, owned AttributeSpecs is filled
     * from Database
     */
    FileCatalog(const QString& dsn, const QString& storage);

    FileCatalog(shared_ptr<Database> db,
                shared_ptr<oh5::AttributeSpecs> specs,
                shared_ptr<FileNamer> namer,
                const QString& storage);

    /**
     * @brief destructor
     */
    ~FileCatalog();

    /**
     * @brief has file been imported to this catalog
     * @param path - absolute path to file
     * @throw db_error if database query fails
     * @throw fs_error if file can not be opened
     * @return true if file is cataloged
     *
     * a file is cataloged when a file with the same unique identifier is
     * already present in database. For the exact rules on how the unique id
     * is determined, refer to File documentation.
     *
     * @sa File::unique_identifier
     */
    bool is_cataloged(const QString& path) const;
    
    /**
     * @brief import file to catalog
     * @param path absolute path to file
     * @return File instance stored in database
     * @throw db_error if storing file to database fails
     * @throw fs_error if file can not be opened or copy to new destination
                       fails.
     * @throw duplicate_entry if file has already been cataloged
     *
     * on import file is physically copied to a new location
     */
    shared_ptr<const oh5::File> catalog(const QString& path);
    
    /**
     * @brief remove file from catalog
     * @param path absolute path to file
     * @throw db_error if removing file entry from database fails
     * @throw fs_error if removing file entry from filesystem fails
     */
    void remove(const QString& path);

    /**
     * @brief get a query object bound to owned database
     */
    Query query() const;

    /**
     * @brief remove all files from this catalog
     */
    void clear();
    
    /**
     * @brief bring this FileCatalog in sync with storage
     */
    //void sync();
    
  private:
    bool is_cataloged(const oh5::File& f) const;

    void check_storage() const;
    
    shared_ptr<Database> db_;
    shared_ptr<oh5::AttributeSpecs> specs_;
    shared_ptr<FileNamer> namer_;
    QDir storage_;
};

}

#endif // BRFC_FILE_CATALOGER_H
