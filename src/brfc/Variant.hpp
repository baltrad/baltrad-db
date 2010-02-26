#ifndef BRFC_VARIANT_HPP
#define BRFC_VARIANT_HPP

#include <brfc/smart_ptr.hpp>
#include <brfc/exceptions.hpp>

#include <boost/variant.hpp>

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QDate>
#include <QtCore/QTime>

#include <string>

class QVariant;

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

    Variant(const QVariant& other);

    explicit Variant(const char* value)
            : type_(STRING)
            , value_(QString::fromUtf8(value)) {
    }

    explicit Variant(const QString& value)
            : type_(STRING)
            , value_(value) {
    }

    explicit Variant(int value)
            : type_(LONGLONG)
            , value_(static_cast<long long>(value)) {
    
    }

    explicit Variant(long long value)
            : type_(LONGLONG)
            , value_(value) {
    }

    explicit Variant(double value)
            : type_(DOUBLE)
            , value_(value) {
    }

    explicit Variant(bool value)
            : type_(BOOL)
            , value_(value) {
    }

    explicit Variant(const QDate& value)
            : type_(DATE)
            , value_(value) {
    }

    explicit Variant(const QTime& value)
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

    const QString& string() const {
        return get<const QString&>();
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

    const QDate& date() const {
        return get<const QDate&>();
    }

    const QTime& time() const {
        return get<const QTime&>();
    }

    QVariant to_qvariant() const;

    QString to_string() const;

  private:
    friend bool operator==(const Variant&, const Variant&);

    template<typename T>
    T get() const {
        try {
            return boost::get<T>(value_);
        } catch (const boost::bad_get&) {
            throw value_error("held variant (" +
                              std::string(value_.type().name()) +
                              ") is not of requested type (" +
                              std::string(typeid(T).name()) + ")");
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

bool operator==(const Variant& lhs, const Variant& rhs);

inline
bool operator!=(const Variant& lhs, const Variant& rhs) {
    return not (lhs == rhs);
}

class variant_to_qvariant : public boost::static_visitor<QVariant> {
  public:
    template<typename T>
    QVariant operator()(const T& value) const {
        return QVariant(value);
    }
};

} // namespace brfc

#endif // BRFC_VARIANT_HPP
