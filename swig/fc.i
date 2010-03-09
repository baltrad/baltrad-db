%module fc

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "common.i"
%import "fc_oh5.i"
%import "fc_expr.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
    #include <brfc/Source.hpp>
    #include <brfc/SourceCentre.hpp>
    #include <brfc/SourceRadar.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/Query.hpp>
    #include <brfc/ResultSet.hpp>
    #include <brfc/Database.hpp>
    #include <brfc/Variant.hpp>
    #include <brfc/TestSwig.hpp>
    
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
%ignore brfc::AttributeSpecs;
%ignore brfc::Database;
%ignore brfc::FileCatalog::FileCatalog(shared_ptr<Database>,
                                       shared_ptr<AttributeSpecs>,
                                       shared_ptr<FileNamer>,
                                       const QString&);

%ignore brfc::Query::Query;
%ignore brfc::ResultSet::ResultSet;

%ignore brfc::ResultSet::operator=;
%rename(boolean_) brfc::ResultSet::boolean;

%ignore brfc::Variant::operator=;

// ignore Source members
%ignore brfc::Source::Source;
%ignore brfc::Source::from_source_attribute;

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

SWIG_SHARED_PTR(Source, brfc::Source);
SWIG_SHARED_PTR_DERIVED(SourceRadar, brfc::Source, brfc::SourceRadar);
SWIG_SHARED_PTR_DERIVED(SourceCentre, brfc::Source, brfc::SourceCentre);

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

%typemap(javabody) brfc::Source %{
  private long swigCPtr;
  private boolean swigCMemOwnBase;

  public $javaclassname(long cPtr, boolean cMemoryOwn) {
    swigCMemOwnBase = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
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
%include <brfc/Source.hpp>
%include <brfc/SourceCentre.hpp>
%include <brfc/SourceRadar.hpp>
%include <brfc/Variant.hpp>
%include <brfc/TestSwig.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
