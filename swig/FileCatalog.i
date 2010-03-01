%module fc

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "boost_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "QString.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
    #include <brfc/Source.hpp>
    #include <brfc/SourceCentre.hpp>
    #include <brfc/SourceRadar.hpp>
    #include <brfc/Attribute.hpp>
    #include <brfc/DataObject.hpp>
    #include <brfc/File.hpp>
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

%ignore brfc::AttributeMapper;
%ignore brfc::AttributeSpecs;
%ignore brfc::Database;
%ignore brfc::FileCatalog::FileCatalog(shared_ptr<Database>,
                                       shared_ptr<AttributeSpecs>,
                                       shared_ptr<FileNamer>);

%ignore brfc::Query::Query;
%ignore brfc::ResultSet::ResultSet;

%ignore brfc::ResultSet::time; // replaced
%ignore brfc::ResultSet::date; // replaced
%ignore brfc::ResultSet::operator=;
%rename(boolean_) brfc::ResultSet::boolean;

%ignore brfc::Variant::time; // replaced
%ignore brfc::Variant::date; // replaced
%ignore brfc::Variant::operator=;

%ignore brfc::expr::Visitor;
%ignore brfc::expr::Compiler;
%ignore brfc::expr::Select;

%ignore brfc::expr::Element::accept;
%ignore brfc::expr::Expression::shared_from_this;
%ignore brfc::expr::Factory::string(const char* value) const;
%ignore brfc::expr::Literal::create;
%ignore brfc::expr::Literal::value;
%ignore brfc::expr::BinaryOperator::create;
%ignore brfc::expr::Attribute::create;
%ignore brfc::expr::Parentheses::create;
%ignore brfc::expr::Label::create;

// ignore File members
%ignore brfc::File::File;
%ignore brfc::File::db_id(long long db_id);

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

%rename(ExpressionFactory) brfc::expr::Factory;
%rename(AttributeExpr) brfc::expr::Attribute;
%rename(boolean_) brfc::expr::Factory::boolean;


SWIG_SHARED_PTR(Element,
                brfc::expr::Element)
SWIG_SHARED_PTR_DERIVED(Expression,
                        brfc::expr::Element,
                        brfc::expr::Expression);
SWIG_SHARED_PTR_DERIVED(BinaryOperator,
                        brfc::expr::Expression,
                        brfc::expr::BinaryOperator);
SWIG_SHARED_PTR_DERIVED(BinaryOperator,
                        brfc::expr::Expression,
                        brfc::expr::BinaryOperator);
SWIG_SHARED_PTR_DERIVED(Literal,
                        brfc::expr::Expression,
                        brfc::expr::Literal);
SWIG_SHARED_PTR_DERIVED(Label,
                        brfc::expr::Expression,
                        brfc::expr::Label);
SWIG_SHARED_PTR_DERIVED(Parentheses,
                        brfc::expr::Expression,
                        brfc::expr::Parentheses);
SWIG_SHARED_PTR_DERIVED(AttributeExpr,
                        brfc::expr::Expression,
                        brfc::expr::Attribute);

SWIG_SHARED_PTR(ResultSet, brfc::ResultSet);
SWIG_SHARED_PTR(File, brfc::File);
SWIG_SHARED_PTR(Attribute, brfc::Attribute);

SWIG_SHARED_PTR(Source, brfc::Source);
SWIG_SHARED_PTR_DERIVED(SourceRadar, brfc::Source, brfc::SourceRadar);
SWIG_SHARED_PTR_DERIVED(SourceCentre, brfc::Source, brfc::SourceCentre);

%template(AttributeVector) std::vector<brfc::shared_ptr<brfc::Attribute> >;
%template(StringVector) std::vector<QString>;

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

%exception {
    try {
        $action
    } catch (const brfc::db_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/DatabaseError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::fs_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/FileSystemError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::lookup_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/LookupError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::duplicate_entry& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/DuplicateEntry");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::brfc_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/FileCatalogError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    }
}

%extend brfc::ResultSet {
    QString _time_string(unsigned int pos) const {
        return $self->time(pos).toString("HH:mm:ss");
    }

    QString _date_string(unsigned int pos) const {
        return $self->date(pos).toString("yyyy-MM-dd");
    }
}

%typemap(javacode) brfc::ResultSet %{
  public java.sql.Date date(long pos) {
    return java.sql.Date.valueOf(_date_string(pos));
  }

  public java.sql.Time time(long pos) {
    return java.sql.Time.valueOf(_time_string(pos));
  }
%}

%extend brfc::Variant {
    QString _time_string() const {
        return $self->time().toString("HH:mm:ss");
    }

    QString _date_string() const {
        return $self->date().toString("yyyy-MM-dd");
    }
}

%typemap(javacode) brfc::Variant %{
  public java.sql.Date date() {
    return java.sql.Date.valueOf(_date_string());
  }

  public java.sql.Time time() {
    return java.sql.Time.valueOf(_time_string());
  }
%}

%include <brfc/expr/fwd.hpp>
%include <brfc/expr/Element.hpp>
%include <brfc/expr/Label.hpp>
%include <brfc/expr/Expression.hpp>
%include <brfc/expr/Factory.hpp>
%include <brfc/expr/Attribute.hpp>
%include <brfc/expr/Parentheses.hpp>
%include <brfc/expr/BinaryOperator.hpp>
%include <brfc/expr/Literal.hpp>

%include <brfc/ResultSet.hpp>
%include <brfc/Database.hpp>
%include <brfc/Attribute.hpp>
%include <brfc/DataObject.hpp>
%include <brfc/File.hpp>
%include <brfc/FileCatalog.hpp>
%include <brfc/Query.hpp>
%include <brfc/Source.hpp>
%include <brfc/SourceCentre.hpp>
%include <brfc/SourceRadar.hpp>
%include <brfc/Variant.hpp>
%include <brfc/TestSwig.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
