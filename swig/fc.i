/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "common.i"
%import "fc_oh5.i"
%import "fc_expr.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/FileEntry.hpp>
    #include <brfc/FileNamer.hpp>
    #include <brfc/Query.hpp>
    #include <brfc/ResultSet.hpp>
    #include <brfc/Database.hpp>
    #include <brfc/Date.hpp>
    #include <brfc/DateTime.hpp>
    #include <brfc/StringList.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/Variant.hpp>
%}

%typemap("javapackage") brfc::oh5::File,
                        brfc::oh5::File*,
                        brfc::oh5::File& "eu.baltrad.fc.oh5";

%feature("director") brfc::FileNamer;

%typemap(javabody) brfc::Date,
                   brfc::DateTime,
                   brfc::StringList,
                   brfc::Time,
                   brfc::Variant,
                   std::vector<brfc::String> %{
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

%ignore brfc::FileCatalog::FileCatalog(shared_ptr<Database>,
                                       shared_ptr<FileNamer>,
                                       const String&);
%ignore brfc::FileCatalog::file_namer(shared_ptr<FileNamer>);

%ignore brfc::Query::Query;
%ignore brfc::ResultSet::ResultSet;

%ignore brfc::ResultSet::operator=;

// uses int references
%ignore brfc::Date::date_from_jdn;

%ignore brfc::StringList::iterator;
%ignore brfc::StringList::const_iterator;
%ignore brfc::StringList::begin;
%ignore brfc::StringList::end;
%ignore brfc::StringList::front(); // nonconst
%ignore brfc::StringList::back(); // nonconst

// ignore Variant members
%ignore brfc::Variant::Variant(const char* value);
%ignore brfc::Variant::operator=;
// deprecated
%ignore brfc::Variant::is_long;
%ignore brfc::Variant::longlong;

%typemap(javacode) brfc::Variant %{
  @Deprecated
  public boolean is_long() {
    return fcJNI.Variant_is_int64(swigCPtr, this);
  }
    
  @Deprecated
  public long longlong() {
    return fcJNI.Variant_int64_(swigCPtr, this);
  }
%}

SWIG_SHARED_PTR(Database, brfc::Database);
SWIG_SHARED_PTR(ResultSet, brfc::ResultSet);
SWIG_SHARED_PTR(FileEntry, brfc::FileEntry);

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.expr.Expression;
    import eu.baltrad.fc.expr.AttributeExpr;
    import eu.baltrad.fc.expr.AttributeExprVector;
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.Source;
%}

%typemap(javaimports) brfc::Query, brfc::Query* %{
    import eu.baltrad.fc.expr.Expression;
    import eu.baltrad.fc.expr.AttributeExpr;
    import eu.baltrad.fc.expr.AttributeExprVector;
%}

%typemap(javaimports) brfc::FileCatalog, brfc::FileCatalog* %{
    import eu.baltrad.fc.oh5.File;
%}

%typemap(javaimports) brfc::FileEntry, brfc::FileEntry* %{
    import eu.baltrad.fc.oh5.File;
%}

%typemap(javaimports) brfc::FileNamer, brfc::FileNamer* %{
    import eu.baltrad.fc.oh5.File;
%}

%typemap(javaimports) brfc::Database, brfc::Database* %{
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.Source;
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

%include <brfc/ResultSet.hpp>
%include <brfc/Database.hpp>
%include <brfc/FileEntry.hpp>
%include <brfc/FileNamer.hpp>
%include <brfc/FileCatalog.hpp>
%include <brfc/Query.hpp>
%include <brfc/Date.hpp>
%include <brfc/Time.hpp>
%include <brfc/DateTime.hpp>
%include <brfc/StringList.hpp>
%include <brfc/Variant.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
