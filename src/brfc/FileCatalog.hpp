#ifndef BRFC_FILE_CATALOGER_H
#define BRFC_FILE_CATALOGER_H

#include <brfc/smart_ptr.hpp>

#include <string>

/**
 * @brief main namespace
 */
namespace brfc {
    
class AttributeSpecs;
class Database;
class File;
class Query;

class FileNamer {
  public:
    std::string name(const File& f) const {
        return do_name(f);
    }

  protected:
    virtual std::string do_name(const File& f) const = 0;
};

class DefaultFileNamer : public FileNamer {
  public:
    DefaultFileNamer(const std::string& path);

  protected:
    virtual std::string do_name(const File& f) const;
  
  private:
    std::string path_;
};

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
    FileCatalog(const std::string& dsn, const std::string& storage);

    FileCatalog(shared_ptr<Database> db,
                const std::string& storage,
                shared_ptr<AttributeSpecs> specs,
                shared_ptr<FileNamer> namer);

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
     * a file is cataloged when a file with a proposed storage path is
     * already present in database. For the exact rules on how the path is
     * determined, refer to File documentation.
     */
    bool is_cataloged(const std::string& path) const;
    
    /**
     * @brief import file to catalog
     * @param path absolute path to file
     * @return path to the stored file (relative to storage root)
     * @throw db_error if storing file to database fails
     * @throw fs_error if file can not be opened or copy to new destination
                       fails.
     * @throw duplicate_entry if file has already been cataloged
     *
     * on import file is physically copied to a new location
     */
    std::string catalog(const std::string& path);
    
    /**
     * @brief remove file from catalog
     * @param path absolute path to file
     * @throw db_error if removing file entry from database fails
     * @throw fs_error if removing file entry from filesystem fails
     */
    void remove(const std::string& path);

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
    bool is_cataloged(const File& f) const;
    
    shared_ptr<Database> db_;
    shared_ptr<AttributeSpecs> specs_;
    shared_ptr<FileNamer> namer_;
};

}

#endif // BRFC_FILE_CATALOGER_H
