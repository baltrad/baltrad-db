#ifndef BRFC_DATABASE_H
#define BRFC_DATABASE_H

#include <boost/noncopyable.hpp>

#include <brfc/smart_ptr.hpp>

class QString;
class QVariant;

namespace brfc {

class AttributeMapper;
class File;
class ResultSet;
class Query;
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
     * @brief remove file from database
     */
    void remove_file(const char* path) {
        do_remove_file(path);
    }
    
    /**
     * @brief save File to database
     */
    void save_file(const File& file, const AttributeMapper& mapper) {
        do_save_file(file, mapper);
    }

    /**
     * @brief execute a select query
     */
    shared_ptr<ResultSet> query(const Query& query) {
        return do_query(query);
    }
    
    /**
     * @brief query populate AttributeMapper with values from database
     */
    void populate_attribute_mapper(AttributeMapper& mapper) {
        return do_populate_attribute_mapper(mapper);
    }
    
    /**
     * @brief delete all files from database
     */
    void clean() {
        do_clean();
    }

  protected:
    virtual void do_begin() = 0;
    virtual void do_rollback() = 0;
    virtual void do_commit() = 0;

    virtual void do_remove_file(const char* path) = 0;
    virtual void do_save_file(const File& file,
                              const AttributeMapper& mapper) = 0;

    virtual shared_ptr<ResultSet> do_query(const Query& query) = 0;

    virtual void do_populate_attribute_mapper(AttributeMapper& mapper) = 0;

    virtual void do_clean() = 0;
};

} // namespace brfc

#endif // BRFC_DATABASE_H
