/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
*/

%module(directors="1") fc

%include "enums.swg"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"


%{
    #include <sstream>

    #include <brfc/exceptions.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/FileNamer.hpp>
    #include <brfc/DefaultFileNamer.hpp>
    #include <brfc/Date.hpp>
    #include <brfc/DateTime.hpp>
    #include <brfc/LocalStorage.hpp>
    #include <brfc/CacheDirStorage.hpp>
    #include <brfc/NullStorage.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/TimeDelta.hpp>
    #include <brfc/Variant.hpp>

    #include <brfc/expr/AttributePrototypes.hpp>
    #include <brfc/expr/Expression.hpp>
    #include <brfc/expr/ExpressionFactory.hpp>

    #include <brfc/oh5/Oh5Node.hpp>
    #include <brfc/oh5/Oh5Scalar.hpp>
    #include <brfc/oh5/Oh5Attribute.hpp>
    #include <brfc/oh5/Oh5File.hpp>
    #include <brfc/oh5/Oh5MetadataMatcher.hpp>
    #include <brfc/oh5/Oh5Group.hpp>
    #include <brfc/oh5/Oh5Source.hpp>
    #include <brfc/oh5/Oh5FileReader.hpp>
    #include <brfc/oh5/Oh5FileWriter.hpp>
    #include <brfc/oh5/Oh5Metadata.hpp>
    #include <brfc/oh5/hl/Oh5HlFileReader.hpp>
    #include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

    #include <brfc/db/AttributeQuery.hpp>
    #include <brfc/db/AttributeResult.hpp>
    #include <brfc/db/Database.hpp>
    #include <brfc/db/FileEntry.hpp>
    #include <brfc/db/FileQuery.hpp>
    #include <brfc/db/FileResult.hpp>

    #include <brfc/test/TestRDB.hpp>
    #include <brfc/test/TempDir.hpp>
    #include <brfc/test/misc.hpp>
%}

%ignore boost::noncopyable;
namespace boost {
    class noncopyable {};
}

#define DEPRECATED(func) func

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
    } catch (const std::invalid_argument& e) {
        jclass cls = jenv->FindClass("java/lang/IllegalArgumentException");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const std::exception& e) {
        jclass cls = jenv->FindClass("java/lang/RuntimeException");
        jenv->ThrowNew(cls, e.what());
        return $null;
    }

}

%feature("director") brfc::FileNamer;

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

/***
 * brfc::Date
 */
SWIG_JAVABODY_METHODS(public, public, brfc::Date);
%ignore brfc::Date::date_from_jdn; // uses int references
%ignore brfc::Date::operator=;
%ignore brfc::Date::operator+=;
%ignore brfc::Date::operator!=;

%rename(add) brfc::Date::operator+;

// brfc::Date::operator==() -> eu.baltrad.fc.Date.equals
%rename(_op_eq) brfc::Date::operator==;
%rename(_op_lt) brfc::Date::operator<;

%typemap(javainterfaces) brfc::Date "Comparable<Date>";

%typemap(javacode) brfc::Date %{
  public boolean equals(Object other) {
    if (other instanceof Date && other.getClass() == Date.class) {
      return _op_eq((Date)other);
    }
    return false;
  }

  public int compareTo(Date other) {
    if (_op_eq(other)) {
        return 0;
    } else if (_op_lt(other)) {
        return -1;
    } else {
        return 1;
    }
  }
%}

/***
 * brfc::DateTime
 */
SWIG_JAVABODY_METHODS(public, public, brfc::DateTime);
%ignore brfc::DateTime::operator=;
%ignore brfc::DateTime::operator+=;
%ignore brfc::DateTime::operator!=;

%rename(add) brfc::DateTime::operator+;

// brfc::Datetime::operator==() -> eu.baltrad.fc.DateTime.equals
%rename(_op_eq) brfc::DateTime::operator==;
%rename(_op_lt) brfc::DateTime::operator<;

%typemap(javainterfaces) brfc::DateTime "Comparable<DateTime>";

%typemap(javacode) brfc::DateTime %{
  public boolean equals(Object other) {
    if (other instanceof DateTime && other.getClass() == DateTime.class) {
      return _op_eq((DateTime)other);
    }
    return false;
  }

  public int compareTo(DateTime other) {
    if (_op_eq(other)) {
        return 0;
    } else if (_op_lt(other)) {
        return -1;
    } else {
        return 1;
    }
  }
%}

/***
 * brfc::Time
 */
SWIG_JAVABODY_METHODS(public, public, brfc::Time);
%ignore brfc::Time::operator=;
%ignore brfc::Time::operator+=;
%ignore brfc::Time::operator!=;

%rename(add) brfc::Time::operator+;

// brfc::Time::operator==() -> eu.baltrad.fc.Time.equals
%rename(_op_eq) brfc::Time::operator==;
%rename(_op_lt) brfc::Time::operator<;

%typemap(javainterfaces) brfc::Time "Comparable<Time>";

%typemap(javacode) brfc::Time %{
  public boolean equals(Object other) {
    if (other instanceof Time && other.getClass() == Time.class) {
      return _op_eq((Time)other);
    }
    return false;
  }

  public int compareTo(Time other) {
    if (_op_eq(other)) {
        return 0;
    } else if (_op_lt(other)) {
        return -1;
    } else {
        return 1;
    }
  }
%}

/***
 * brfc::TimeDelta
 */
SWIG_JAVABODY_METHODS(public, public, brfc::TimeDelta);

/***
 * std::vector<std::string>
 */
SWIG_JAVABODY_METHODS(public, public, std::vector<std::string>);
%template(_StdVectorString) std::vector<std::string>;

/***
 * brfc::Variant
 */
SWIG_JAVABODY_METHODS(public, public, brfc::Variant);
%ignore brfc::Variant::Variant(unsigned int);
%ignore brfc::Variant::Variant(const char* value);
%ignore brfc::Variant::operator=;

/***
 * brfc::CacheDirStorage
 **/
%ignore brfc::CacheDirStorage::CacheDirStorage(const std::string&, const FileSystem*);

/***
 * brfc::FileCatalog
 */
%newobject brfc::FileCatalog::store;
%newobject brfc::FileCatalog::get_or_store;

/***
 * brfc::FileEntry
 */
SWIG_JAVABODY_METHODS(public, public, brfc::FileEntry);

/***
 * brfc::FileQuery
 */
%ignore brfc::FileQuery::order() const;

%newobject brfc::FileQuery::execute;

/***
 * brfc::FileResult
 */
%ignore brfc::FileResult::FileResult;
%ignore brfc::FileResult::operator=;

SWIG_JAVABODY_METHODS(public, public, brfc::FileResult);

%newobject brfc::FileResult::entry;

/***
 * brfc::AttributeQuery
 */
%ignore brfc::AttributeQuery::fetch() const;
%ignore brfc::AttributeQuery::group() const;
%ignore brfc::AttributeQuery::order() const;

%newobject brfc::AttributeQuery::execute;

/***
 * brfc::AttributeResult
 */
SWIG_JAVABODY_METHODS(public, public, brfc::AttributeResult);

/***
 * brfc::Database
 */

%newobject brfc::Database::create;
%newobject brfc::Database::entry_by_file;
%newobject brfc::Database::entry_by_uuid;
%newobject brfc::Database::execute;
%newobject brfc::Database::get_or_store;
%newobject brfc::Database::store;

SWIG_JAVABODY_METHODS(public, public, brfc::Database);

%typemap(javaimports) brfc::Database, brfc::Database* %{
    import java.util.ArrayList;
    import java.util.List;
%}

// brfc::Database::sources -> eu.baltrad.fc.db.Database._sources
%rename(_sources) brfc::Database::sources;
%typemap(javacode) brfc::Database %{
  public List<Oh5Source> sources() {
    _StdVectorOh5Source srcvec = _sources();
    List<Oh5Source> srclist = new ArrayList<Oh5Source>((int)srcvec.size());
    for (int i=0; i < srcvec.size(); i++) {
        srclist.add(srcvec.get(i));
    }
    return srclist;
  }
%}

%typemap(javabody) brfc::AttributeQuery,
                   brfc::FileQuery %{ 
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

/***
 * brfc::Expression
 */
%extend brfc::Expression {
    std::string toString() {
        std::stringstream ss;
        ss << *$self;
        return ss.str();
    }
};

%ignore brfc::Expression::Expression(const std::string& value, construct_symbol);
%ignore brfc::Expression::Expression(const list_t& );
%ignore brfc::Expression::type() const;
%ignore brfc::Expression::list() const;
%ignore brfc::Expression::begin;
%ignore brfc::Expression::end;
%ignore brfc::PrintTo;

%rename(equals) brfc::operator==(const Expression&, const Expression&);

%typemap(javaimports) brfc::Expression %{
  import java.util.Collection;
%}

SWIG_JAVABODY_METHODS(public, public, brfc::Expression)

%typemap(javacode) brfc::Expression %{
  public Expression(Collection<Expression> values) {
    this();
    for (Expression e : values) {
      push_back(e);
    }
  }

  public Expression(Expression[] values) {
    this();
    for (int i=0; i < values.length; i++) {
      push_back(values[i]);
    }
  }
  
  @Override
  public boolean equals(Object o) {
    if (o instanceof Expression) {
        return fc.equals(this, (Expression)o);
    } else {
        return false;
    }
  }
%}

/***
 * brfc::ExpressionFactory
 */
%ignore brfc::ExpressionFactory::string(const char* value) const;
%typemap(javaimports) brfc::ExpressionFactory %{
  import java.util.Collection;
%}

/***
 * brfc::Oh5Node
 */
%ignore brfc::Oh5Node::Oh5Node;
%ignore brfc::Oh5Node::begin;
%ignore brfc::Oh5Node::end;
%ignore brfc::Oh5Node::backend;
%ignore brfc::Oh5Node::children() const;
%ignore brfc::Oh5Node::parent() const;
%ignore brfc::Oh5Node::root() const;
%ignore brfc::Oh5Node::child(const std::string&) const;
%ignore brfc::Oh5Node::file() const;

%delobject brfc::Oh5Node::add;

%template(Oh5NodeVector) std::vector<brfc::Oh5Node* >;

/***
 * brfc::Oh5Group
 */
%ignore brfc::Oh5Group::attribute(const std::string&) const;
%ignore brfc::Oh5Group::effective_attribute(const std::string&) const;
%ignore brfc::Oh5Group::group(const std::string&) const;

/***
 * brfc::Oh5Source
 */
SWIG_JAVABODY_METHODS(public, public, brfc::Oh5Source);

%rename(_keys) brfc::Oh5Source::keys;
%rename(_all_keys) brfc::Oh5Source::all_keys;

%typemap(javaimports) brfc::Oh5Source,
                      brfc::Oh5Source* %{
    import java.util.List;
    import java.util.ArrayList;
%}

%typemap(javacode) brfc::Oh5Source %{
  public List<String> keys() {
    _StdVectorString std_keys = _keys();
    List<String> keys = new ArrayList<String>((int)std_keys.size());
    for (int i=0; i < std_keys.size(); i++) {
      keys.add(std_keys.get(i));
    }
    return keys;
  }

  public List<String> all_keys() {
    _StdVectorString std_keys = _all_keys();
    List<String> keys = new ArrayList<String>((int)std_keys.size());
    for (int i=0; i < std_keys.size(); i++) {
      keys.add(std_keys.get(i));
    }
    return keys;
  }
%}

/***
 * std::vector<brfc::Oh5Source>
 */
SWIG_JAVABODY_METHODS(public, public, std::vector<brfc::Oh5Source>);
%template(_StdVectorOh5Source) std::vector<brfc::Oh5Source>;

/***
 * brfc::Oh5Scalar
 */
%ignore brfc::Oh5Scalar::Oh5Scalar(const char* value);
%ignore brfc::Oh5Scalar::operator=;

/***
 * brfc::Oh5File
 */
%ignore brfc::Oh5File::group(const brfc::string&) const;
%ignore brfc::Oh5File::root() const;

SWIG_JAVABODY_METHODS(public, public, brfc::Oh5File);

%include <brfc/FileNamer.hpp>
%include <brfc/DefaultFileNamer.hpp>
%include <brfc/FileCatalog.hpp>
%include <brfc/LocalStorage.hpp>
%include <brfc/CacheDirStorage.hpp>
%include <brfc/NullStorage.hpp>
%include <brfc/TimeDelta.hpp>
%include <brfc/Date.hpp>
%include <brfc/Time.hpp>
%include <brfc/DateTime.hpp>
%include <brfc/Variant.hpp>

%include <brfc/expr/AttributePrototypes.hpp>
%include <brfc/expr/Expression.hpp>
%include <brfc/expr/ExpressionFactory.hpp>

%include <brfc/oh5/Oh5File.hpp>
%include <brfc/oh5/Oh5Scalar.hpp>
%include <brfc/oh5/Oh5Node.hpp>
%include <brfc/oh5/Oh5Group.hpp>
%include <brfc/oh5/Oh5Attribute.hpp>
%include <brfc/oh5/Oh5Source.hpp>
%include <brfc/oh5/Oh5MetadataMatcher.hpp>
%include <brfc/oh5/Oh5FileReader.hpp>
%include <brfc/oh5/Oh5FileWriter.hpp>
%include <brfc/oh5/Oh5Metadata.hpp>
%include <brfc/oh5/hl/Oh5HlFileReader.hpp>
%include <brfc/oh5/hl/Oh5HlFileWriter.hpp>

%include <brfc/db/AttributeResult.hpp>
%include <brfc/db/AttributeQuery.hpp>
%include <brfc/db/FileResult.hpp>
%include <brfc/db/Database.hpp>
%include <brfc/db/FileEntry.hpp>
%include <brfc/db/FileQuery.hpp>

%include <brfc/test/TestRDB.hpp>
%include <brfc/test/TempDir.hpp>
%include <brfc/test/misc.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
