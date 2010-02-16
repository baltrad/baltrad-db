#ifndef BRFC_TEST_COMMON_HPP
#define BRFC_TEST_COMMON_HPP

#include <iosfwd>
#include <map>

#include <gtest/gtest.h>

class QVariant;
class QDate;
class QTime;

namespace brfc {

    class TestRDB;
    class Variant;

class TestRDBEnv : public ::testing::Environment {
  public:
    
    virtual ~TestRDBEnv() { }

    static TestRDB* get_database(const char* dsn);

    virtual void SetUp();

    virtual void TearDown();

  private:
    typedef std::map<const char*, TestRDB*> DatabaseMap;
    static DatabaseMap databases_;
};

} // namespace brfc

std::ostream&
operator<<(std::ostream& out, const QVariant& value);

std::ostream&
operator<<(std::ostream& out, const QDate& value);

std::ostream&
operator<<(std::ostream& out, const QTime& value);

std::ostream&
operator<<(std::ostream& out, const brfc::Variant& value);

#endif // BRFC_TEST_COMMON_HPP
