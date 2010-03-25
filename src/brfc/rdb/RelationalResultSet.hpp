#ifndef BRFC_RELATIONAL_RESULT_SET_HPP
#define BRFC_RELATIONAL_RESULT_SET_HPP

#include <boost/scoped_ptr.hpp>

#include <brfc/ResultSet.hpp>

class QDate;
class QTime;
class QSqlQuery;

namespace brfc {
namespace rdb {

/**
 * @brief results from RelationalDatabase
 */
class RelationalResultSet : public ResultSet {
  public:
    /**
     * @brief construct from QSqlQuery
     *
     * position is before first row.
     */
    explicit RelationalResultSet(const QSqlQuery& query);
    
    /**
     * @brief destructor
     */
    virtual ~RelationalResultSet();
  
  protected:
    virtual bool do_next();
    virtual bool do_seek(int idx);
    virtual int do_size();

    Variant do_value_at(unsigned int pos) const;
  
  private:
    boost::scoped_ptr<QSqlQuery> query_;
};

} // namespace rdb
} // namespace brfc

#endif // BRFC_RELATIONAL_RESULT_SET_HPP