#ifndef BRFC_RESULT_SET_HPP
#define BRFC_RESULT_SET_HPP

#include <boost/noncopyable.hpp>

class QString;
class QDate;
class QTime;

namespace brfc {

class Variant;

/**
 * @brief ABC for Query results
 * @ingroup exposed_in_binds
 */
class ResultSet : public boost::noncopyable {
  public:
    /**
     * @brief destructor
     */
    virtual ~ResultSet() { }

    /**
     * @name seeking
     * @{
     */
    /**
     * @brief move to next row
     * @return true if new row is valid
     */
    bool next() {
        return do_next();
    }
    
    /**
     * @brief seek to row
     * @param idx row number, starting from 0. if negative, before first row.
     * @return true if new row is valid
     */
    bool seek(int idx) {
        return do_seek(idx);
    }

    /**
     * @brief get number of rows in result
     */
    int size() {
        return do_size();
    }
    //@}
    
    /**
     * @name value access
     * @{
     */
    /**
     * @brief get string value at \c pos
     */
    QString string(unsigned int pos) const;

    /**
     * @brief get integer value at \c pos
     */
    long long integer(unsigned int pos) const;

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

    /**
     * @brief get value at \c pos as Variant
     */
    Variant value(unsigned int pos) const;
    //@}
  
  protected:
    virtual bool do_next() = 0;
    virtual bool do_seek(int idx) = 0;
    virtual int do_size() = 0;
    
    Variant value_at(unsigned int pos) const;
    virtual Variant do_value_at(unsigned int pos) const = 0;
};

}

#endif // BRFC_RESULT_SET_HPP
