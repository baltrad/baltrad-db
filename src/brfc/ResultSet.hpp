#ifndef BRFC_RESULT_SET_HPP
#define BRFC_RESULT_SET_HPP

#include <boost/scoped_ptr.hpp>

#include <string>

class QDate;
class QTime;
class QSqlQuery;
class QVariant;

namespace brfc {

/**
 * @brief wrapper around QSqlQuery
 * @ingroup exposed_in_binds
 */
class ResultSet {
  public:
    /**
     * @brief construct from QSqlQuery
     *
     * position is before first row.
     */
    explicit ResultSet(const QSqlQuery& query);
    
    /**
     * @brief copy constructor
     */
    ResultSet(const ResultSet& other);

    /**
     * @brief copy assignment
     */
    ResultSet& operator=(const ResultSet& rhs);
    
    /**
     * @brief destructor
     */
    ~ResultSet();

    /**
     * @name seeking
     * @{
     */
    /**
     * @brief move to next row
     * @return true if new row is valid
     */
    bool next();
    
    /**
     * @brief seek to row
     * @param idx row number, starting from 0. if negative, before first row.
     * @return true if new row is valid
     */
    bool seek(int idx);

    /**
     * @brief get number of rows in result
     */
    int size();
    //@}
    
    /**
     * @name value access
     * @{
     */
    /**
     * @brief get string value at \c pos
     */
    std::string string(unsigned int pos) const;

    /**
     * @brief get integer value at \c pos
     */
    int64_t integer(unsigned int pos) const;

    /**
     * @brief get real value at \c pos
     */
    double real(unsigned int pos) const;

    /**
     * @brief get boolean value at \c pos
     */
    bool boolean(unsigned int pos) const;

    /**
     * @brief get date value at \c pos
     */
    QDate date(unsigned int pos) const;

    /**
     * @brief get time value at \c pos
     */
    QTime time(unsigned int pos) const;
    //@}

  private:
    QVariant value_at(unsigned int pos) const;

    boost::scoped_ptr<QSqlQuery> query_;
};

} // namespace brfc

#endif // BRFC_RESULT_SET_HPP
