#ifndef BRFC_DATABASE_H
#define BRFC_DATABASE_H

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

class QString;

namespace brfc {

class ResultSet;
class Query;

namespace oh5 {

class File;
class Source;

}

/**
 * @brief ABC for Database access
 */
class Database : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~Database() { }
    
    /**
     * @brief start a transaction
     */
    void begin() {
        do_begin();
    }

    /**
     * @brief roll back current transaction
     */
    void rollback() {
        do_rollback();
    }

    /**
     * @brief commit current transaction
     */
    void commit() {
        do_commit();
    }

    /**
     * @brief is file stored
     */
    bool has_file(const oh5::File& file) {
        return do_has_file(file);
    }
    
    /**
     * @brief remove file from database
     */
    void remove_file(const QString& path) {
        do_remove_file(path);
    }
    
    /**
     * @brief save file to database
     * @return database id associated with the file
     */
    long long save_file(const oh5::File& file) {
        return do_save_file(file);
    }

    /**
     * @brief load source from DB by contents of /what/source
     */
    shared_ptr<oh5::Source> load_source(const QString& source) {
        return do_load_source(source);
    }
    
    /**
     * @brief execute a select query
     */
    shared_ptr<ResultSet> query(const Query& query) {
        return do_query(query);
    }

  protected:
    virtual void do_begin() = 0;
    virtual void do_rollback() = 0;
    virtual void do_commit() = 0;
    
    virtual bool do_has_file(const oh5::File& file) = 0;
    virtual void do_remove_file(const QString& path) = 0;
    virtual long long do_save_file(const oh5::File& file) = 0;

    virtual shared_ptr<oh5::Source> do_load_source(const QString& source) = 0;

    virtual shared_ptr<ResultSet> do_query(const Query& query) = 0;
};

} // namespace brfc

#endif // BRFC_DATABASE_H
