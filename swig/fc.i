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

%include "common.i"

%{
    #include <brfc/exceptions.hpp>
    #include <brfc/FileCatalog.hpp>
    #include <brfc/FileNamer.hpp>
    #include <brfc/Date.hpp>
    #include <brfc/DateTime.hpp>
    #include <brfc/LocalStorage.hpp>
    #include <brfc/CacheDirStorage.hpp>
    #include <brfc/StringList.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/TimeDelta.hpp>
    #include <brfc/Variant.hpp>
    #include <brfc/db/AttributeQuery.hpp>
    #include <brfc/db/FileQuery.hpp>
%}

%import "fc_db.i"
%import "fc_expr.i"
%import "fc_oh5.i"

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

/***
 * brfc::Date
 */
%ignore brfc::Date::date_from_jdn; // uses int references
%ignore brfc::Date::operator=;
%ignore brfc::Date::operator+=;
%ignore brfc::Date::operator!=;

%rename(add) brfc::Date::operator+;

// brfc::Date::operator==() -> eu.baltrad.fc.Date.equals
%rename(_op_eq) brfc::Date::operator==;
%typemap(javacode) brfc::Date %{
  public boolean equals(Object other) {
    if (other instanceof Date && other.getClass() == Date.class) {
      return _op_eq((Date)other);
    }
    return false;
  }
%}

/***
 * brfc::DateTime
 */
%ignore brfc::DateTime::operator=;
%ignore brfc::DateTime::operator+=;
%ignore brfc::DateTime::operator!=;
%ignore brfc::DateTime::date() const; // use non-const
%ignore brfc::DateTime::time() const; // use non-const

%rename(add) brfc::DateTime::operator+;

// brfc::Datetime::operator==() -> eu.baltrad.fc.DateTime.equals
%rename(_op_eq) brfc::DateTime::operator==;
%typemap(javacode) brfc::DateTime %{
  public boolean equals(Object other) {
    if (other instanceof DateTime && other.getClass() == DateTime.class) {
      return _op_eq((DateTime)other);
    }
    return false;
  }
%}

/***
 * brfc::Time
 */
%ignore brfc::Time::operator=;
%ignore brfc::Time::operator+=;
%ignore brfc::Time::operator!=;

%rename(add) brfc::Time::operator+;

// brfc::Time::operator==() -> eu.baltrad.fc.Time.equals
%rename(_op_eq) brfc::Time::operator==;
%typemap(javacode) brfc::Time %{
  public boolean equals(Object other) {
    if (other instanceof Time && other.getClass() == Time.class) {
      return _op_eq((Time)other);
    }
    return false;
  }
%}

/***
 * brfc::StringList
 */
%ignore brfc::StringList::iterator;
%ignore brfc::StringList::const_iterator;
%ignore brfc::StringList::begin;
%ignore brfc::StringList::end;
%ignore brfc::StringList::front(); // nonconst
%ignore brfc::StringList::back(); // nonconst

/***
 * brfc::Variant
 */
%ignore brfc::Variant::Variant(unsigned int);
%ignore brfc::Variant::Variant(const char* value);
%ignore brfc::Variant::operator=;

/***
 * brfc::LocalStorage
 **/
%typemap(javaimports) brfc::LocalStorage, brfc::LocalStorage* %{
    import eu.baltrad.fc.db.FileEntry;
%}

/***
 * brfc::CacheDirStorage
 **/
%typemap(javaimports) brfc::CacheDirStorage, brfc::CacheDirStorage* %{
    import eu.baltrad.fc.db.FileEntry;
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.db.Database;
    import eu.baltrad.fc.db.FileEntry;
    import eu.baltrad.fc.oh5.File;
    import eu.baltrad.fc.oh5.PhysicalFile;
    import eu.baltrad.fc.oh5.Source;
%}

%typemap(javaimports) brfc::FileCatalog, brfc::FileCatalog* %{
    import eu.baltrad.fc.db.AttributeQuery;
    import eu.baltrad.fc.db.Database;
    import eu.baltrad.fc.db.FileEntry;
    import eu.baltrad.fc.db.FileQuery;
    import eu.baltrad.fc.oh5.PhysicalFile;
%}

%typemap(javaimports) brfc::FileNamer, brfc::FileNamer* %{
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

%include <brfc/FileNamer.hpp>
%include <brfc/FileCatalog.hpp>
%include <brfc/LocalStorage.hpp>
%include <brfc/CacheDirStorage.hpp>
%include <brfc/TimeDelta.hpp>
%include <brfc/Date.hpp>
%include <brfc/Time.hpp>
%include <brfc/DateTime.hpp>
%include <brfc/StringList.hpp>
%include <brfc/Variant.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
