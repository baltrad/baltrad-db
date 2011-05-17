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
 * brfc::FileEntry
 */
%typemap(javaimports) brfc::FileEntry, brfc::FileEntry* %{
    import eu.baltrad.fc.DateTime;
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.Source;
%}

SWIG_JAVABODY_METHODS(public, public, brfc::FileEntry);

/***
 * brfc::FileQuery
 */
%ignore brfc::FileQuery::order() const;

%typemap(javaimports) brfc::FileQuery, brfc::FileQuery* %{
    import eu.baltrad.fc.expr.Expression;
%}

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
%typemap(javaimports) brfc::AttributeQuery, brfc::AttributeQuery* %{
    import eu.baltrad.fc.expr.Expression;
%}

%newobject brfc::AttributeQuery::execute;

/***
 * brfc::AttributeResult
 */
SWIG_JAVABODY_METHODS(public, public, brfc::AttributeResult);

%typemap(javaimports) brfc::AttributeResult, brfc::AttributeResult* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.Variant;
%}


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

    import eu.baltrad.fc.oh5.PhysicalFile;
    import eu.baltrad.fc.oh5.Source;
    import eu.baltrad.fc.oh5._StdVectorSource;
%}

// brfc::Database::sources -> eu.baltrad.fc.db.Database._sources
%rename(_sources) brfc::Database::sources;
%typemap(javacode) brfc::Database %{
  public List<Source> sources() {
    _StdVectorSource srcvec = _sources();
    List<Source> srclist = new ArrayList<Source>((int)srcvec.size());
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


%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.expr.Expression;
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
