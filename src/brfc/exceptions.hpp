#ifndef BRFC_EXCEPTIONS_HPP
#define BRFC_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class QSqlError;

namespace brfc {

/**
 * @defgroup exceptions Exceptions
 * @{
 */
/**
 * @brief base class for all errors from this library
 *
 * equivalent exception in java bindings is @c FileCatalogError
 * @ingroup exposed_in_binds
 */
class brfc_error : public std::runtime_error {
  public:
    explicit brfc_error(const std::string& what)
            : runtime_error(what) {
    }
};

/**
 * @brief object lookup failed
 *
 * equivalent exception in java bindings is @c LookupError
 * @ingroup exposed_in_binds
 */
class lookup_error : public brfc_error {
  public:
    explicit lookup_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief entry already exists
 *
 * equivalent exception in java bindings is @c DuplicateEntry
 * @ingroup exposed_in_binds
 */
class duplicate_entry : public brfc_error {
  public:
    explicit duplicate_entry(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief database query/connection failed
 *
 * equivalent exception in java bindings is @c DatabaseError
 * @ingroup exposed_in_binds
 */
class db_error : public brfc_error {
  public:
    /** 
     * @brief construct from QSqlError
     */ 
    explicit db_error(const QSqlError& error);

    explicit db_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @brief filesystem related error
 *
 * equivalent exception in java bindings is @c FileSystemError
 * @ingroup exposed_in_binds
 */
class fs_error : public brfc_error {
  public:
    explicit fs_error(const std::string& what)
            : brfc_error(what) {
    }
};

/**
 * @} // end of exception group
 */

}

#endif // BRFC_EXCEPTIONS_HPP
