#ifndef BRFC_VARIANT_HPP
#define BRFC_VARIANT_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/exceptions.hpp>

#include <boost/variant.hpp>

#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QTime>

namespace brfc {

class Variant {
  public:
    enum Type {
        NONE,
        STRING,
        LONGLONG,
        DOUBLE,
        BOOL,
        DATE,
        TIME
    };

    Variant()
            : type_(NONE)
            , value_() {
    }

    Variant(const Variant& other)
            : type_(other.type_)
            , value_(other.value_) {
    }

    Variant(const char* value)
            : type_(STRING)
            , value_(QString::fromUtf8(value)) {
    }

    Variant(int value)
            : type_(LONGLONG)
            , value_(static_cast<long long>(value)) {
    
    }

    Variant(long long value)
            : type_(LONGLONG)
            , value_(value) {
    }

    Variant(double value)
            : type_(DOUBLE)
            , value_(value) {
    }

    Variant(bool value)
            : type_(BOOL)
            , value_(value) {
    }

    Variant(const QDate& value)
            : type_(DATE)
            , value_(value) {
    }

    Variant(const QTime& value)
            : type_(TIME)
            , value_(value) {
    }

    Variant& operator=(const Variant& rhs) {
        if (this != &rhs) {
            type_ = rhs.type_;
            value_ = rhs.value_;
        }
        return *this;
    }

    Type type() const { return type_; }

    bool is_null() const { return type_ == NONE; }
    bool is_string() const { return type_ == STRING; }
    bool is_long() const { return type_ == LONGLONG; }
    bool is_double() const { return type_ == DOUBLE; }
    bool is_bool() const { return type_ == BOOL; }
    bool is_date() const { return type_ == DATE; }
    bool is_time() const { return type_ == TIME; }

    const char* string() const {
        return get<QString>().toUtf8();
    }

    long long longlong() const {
        return get<long long>();
    }

    double double_() const {
        return get<double>();
    }

    bool bool_() const {
        return get<bool>();
    }

    QDate date() const {
        return get<QDate>();
    }

    QTime time() const {
        return get<QTime>();
    }

  private:
    template<typename T>
    T get() const {
        try {
            return boost::get<T>(value_);
        } catch (const boost::bad_get& exc) {
            throw value_error(exc.what());
        }
    }

    Type type_;
    typedef boost::variant<QString,
                           long long,
                           double,
                           bool,
                           QDate,
                           QTime> ValueType;
    ValueType value_;
};

} // namespace brfc

#endif // BRFC_VARIANT_HPP
