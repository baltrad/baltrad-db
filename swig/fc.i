%module fc

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "common.i"
%import "fc_oh5.i"
%import "fc_expr.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/Query.hpp>
    #include <brfc/ResultSet.hpp>
    #include <brfc/Database.hpp>
    #include <brfc/Variant.hpp>
    
    #include <brfc/expr/fwd.hpp>
    #include <brfc/expr/Element.hpp>
    #include <brfc/expr/Expression.hpp>
    #include <brfc/expr/Factory.hpp>
    #include <brfc/expr/Attribute.hpp>
    #include <brfc/expr/Parentheses.hpp>
    #include <brfc/expr/Label.hpp>
    #include <brfc/expr/BinaryOperator.hpp>
    #include <brfc/expr/Literal.hpp>

    #include <QtCore/QTime>
    #include <QtCore/QDate>
%} 

%typemap(javabody) QDate,
                   QTime,
                   brfc::Variant,
                   std::vector<QString> %{
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public $javaclassname(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }
%}

%ignore brfc::AttributeMapper;
%ignore brfc::oh5::AttributeSpecs;
%ignore brfc::Database;
%ignore brfc::FileCatalog::FileCatalog(shared_ptr<Database>,
                                       shared_ptr<oh5::AttributeSpecs>,
                                       shared_ptr<FileNamer>,
                                       const QString&);

%ignore brfc::Query::Query;
%ignore brfc::ResultSet::ResultSet;

%ignore brfc::ResultSet::operator=;
%rename(boolean_) brfc::ResultSet::boolean;

%ignore brfc::Variant::operator=;

// ignore Variant members
%ignore brfc::Variant::Variant;
%ignore brfc::operator=;
%ignore brfc::Variant::qstring;
%ignore brfc::Variant::to_qvariant;
%ignore brfc::variant_to_qvariant;

%ignore brfc::Query::fetch;

%rename(Date) QDate;
%rename(Time) QTime;

SWIG_SHARED_PTR(ResultSet, brfc::ResultSet);

%template(StringVector) std::vector<QString>;

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.expr.Expression;
%}

%typemap(javaimports) brfc::Query, brfc::Query* %{
    import eu.baltrad.fc.expr.Expression;
%}

%typemap(javaimports) brfc::FileCatalog, brfc::FileCatalog* %{
    import eu.baltrad.fc.oh5.File;
%}


// Enable the JNI class to load the required native library.
%pragma(java) jniclasscode=%{
  static {
    try {
        System.loadLibrary("brfc_java");
    } catch (UnsatisfiedLinkError e) {
        System.err.println("brfc_java native code library failed to load.\n" + e);
        System.exit(1);
    }
  }
%}

class QDate {
  public:
    QDate(int year, int month, int day);

    int year() const;
    int month() const;
    int day() const;
};


class QTime {
  public:
    QTime(int hour, int minute, int second=0, int ms=0);

    int hour() const;
    int minute() const;
    int second() const;
    int msec() const;
};

%include <QtCore/QDate>
%include <QtCore/QTime>

%include <brfc/ResultSet.hpp>
%include <brfc/Database.hpp>
%include <brfc/FileCatalog.hpp>
%include <brfc/Query.hpp>
%include <brfc/Variant.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
