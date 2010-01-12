%module fc

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "boost_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/Query.hpp>
    #include <brfc/ResultSet.hpp>
    #include <brfc/Database.hpp>
    
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

%ignore brfc::expr::Visitor;
%ignore brfc::expr::Compiler;
%ignore brfc::expr::Select;

%ignore brfc::expr::Element::accept;
%ignore brfc::expr::Expression::shared_from_this;
%ignore brfc::expr::Literal::create;
%ignore brfc::expr::Literal::value;
%ignore brfc::expr::BinaryOperator::create;
%ignore brfc::expr::Attribute::create;
%ignore brfc::expr::Parentheses::create;
%ignore brfc::expr::Label::create;

%rename(ExpressionFactory) brfc::expr::Factory;
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
SWIG_SHARED_PTR_DERIVED(Attribute,
                        brfc::expr::Expression,
                        brfc::expr::Attribute);

SWIG_SHARED_PTR(ResultSet, brfc::ResultSet);

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
    std::string _time_string(unsigned int pos) const {
        return $self->time(pos).toString("HH:mm:ss").toStdString();
    }

    std::string _date_string(unsigned int pos) const {
        return $self->date(pos).toString("yyyy-MM-dd").toStdString();
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
%include <brfc/FileCatalog.hpp>
%include <brfc/Query.hpp>


/* vim:filetype=cpp:et:ts=4:sw=4:
*/
