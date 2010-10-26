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

%module fc_db

%include "common.i"

%{
    #include <brfc/oh5/File.hpp>
    #include <brfc/Variant.hpp>
    #include <brfc/db/AttributeQuery.hpp>
    #include <brfc/db/AttributeResult.hpp>
    #include <brfc/db/Database.hpp>
    #include <brfc/db/FileEntry.hpp>
    #include <brfc/db/FileQuery.hpp>
    #include <brfc/db/FileResult.hpp>
%}

%import "fc.i"
%import "fc_oh5.i"
%import "fc_expr.i"

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
 * brfc::db::FileEntry
 */
SWIG_SHARED_PTR_DERIVED(FileEntry,
                        brfc::oh5::File,
                        brfc::db::FileEntry);

%typemap(javaimports) brfc::db::FileEntry, brfc::db::FileEntry* %{
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.Source;
%}

%typemap(javabody_derived) brfc::db::FileEntry %{
  private long swigCPtr;
  private boolean swigCMemOwnDerived;

  public $javaclassname(long cPtr, boolean cMemoryOwn) {
    super($imclassname.$javaclassname_SWIGSharedPtrUpcast(cPtr), true);
    swigCMemOwnDerived = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr($javaclassname obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }
%}

/***
 * brfc::db::FileQuery
 */
%ignore brfc::db::FileQuery::FileQuery;

%typemap(javaimports) brfc::db::FileQuery, brfc::db::FileQuery* %{
    import eu.baltrad.fc.expr.Expression;
    import eu.baltrad.fc.expr.AttributeExpr;
    import eu.baltrad.fc.expr.AttributeExprVector;
%}

/***
 * brfc::db::FileResult
 */
SWIG_SHARED_PTR(FileResult, brfc::db::FileResult);
%ignore brfc::db::FileResult::FileResult;
%ignore brfc::db::FileResult::operator=;


/***
 * brfc::db::AttributeQuery
 */
%ignore brfc::db::AttributeQuery::order() const;
%ignore brfc::db::AttributeQuery::fetch() const;
%typemap(javaimports) brfc::db::AttributeQuery, brfc::db::AttributeQuery* %{
    import eu.baltrad.fc.expr.AttributeExpr;
    import eu.baltrad.fc.expr.Expression;
    import eu.baltrad.fc.expr.Function;
%}

/**
 * brfc::db::AttributeResult
 */
SWIG_SHARED_PTR(AttributeResult, brfc::db::AttributeResult);

%typemap(javaimports) brfc::db::AttributeResult, brfc::db::AttributeResult* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.Variant;
%}


/***
 * brfc::db::Database
 */
SWIG_SHARED_PTR(Database, brfc::db::Database);

%typemap(javaimports) brfc::db::Database, brfc::db::Database* %{
    import eu.baltrad.fc.oh5.PhysicalFile;
    import eu.baltrad.fc.oh5.Source;
%}

// make constructors for SWIG_SHARED_PTR public
%typemap(javabody) brfc::db::Database,
                   brfc::db::Expression %{
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

%typemap(javabody) brfc::db::AttributeQuery,
                   brfc::db::FileQuery %{ 
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


%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.expr.AttributeExpr;
    import eu.baltrad.fc.expr.AttributeExprVector;
    import eu.baltrad.fc.expr.Expression;
    import eu.baltrad.fc.expr.Function;
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.PhysicalFile;
    import eu.baltrad.fc.oh5.Source;
%}

%include <brfc/db/AttributeResult.hpp>
%include <brfc/db/AttributeQuery.hpp>
%include <brfc/db/FileResult.hpp>
%include <brfc/db/Database.hpp>
%include <brfc/db/FileEntry.hpp>
%include <brfc/db/FileQuery.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
