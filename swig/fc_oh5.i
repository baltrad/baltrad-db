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

%module fc_oh5

%include "common.i"
%import "fc.i"

%{
    #include <brfc/Date.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/StringList.hpp>
    #include <brfc/oh5/Node.hpp>
    #include <brfc/oh5/Scalar.hpp>
    #include <brfc/oh5/Attribute.hpp>
    #include <brfc/oh5/AttributeGroup.hpp>
    #include <brfc/oh5/File.hpp>
    #include <brfc/oh5/Group.hpp>
    #include <brfc/oh5/RootGroup.hpp>
    #include <brfc/oh5/Source.hpp>
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

%ignore brfc::oh5::Node::begin;
%ignore brfc::oh5::Node::end;
%ignore brfc::oh5::Node::add_child;

%ignore brfc::oh5::Group::create_by_name;

%ignore brfc::oh5::Scalar::Scalar(const char* value);

%template(NodeVector) std::vector<brfc::shared_ptr<brfc::oh5::Node> >;
%template(AttributeVector) std::vector<brfc::shared_ptr<brfc::oh5::Attribute> >;

%typemap(javaimports) brfc::oh5::File,
                      brfc::oh5::File* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.StringList;
%}

%typemap(javaimports) brfc::oh5::Group,
                      brfc::oh5::Group*,
                      brfc::oh5::Source,
                      brfc::oh5::Source* %{
    import eu.baltrad.fc.StringList;
%}

%typemap(javaimports) brfc::oh5::Scalar,
                      brfc::oh5::Scalar* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.StringList;
%}

// make constructors public
%typemap(javabody) brfc::oh5::File,
                   brfc::oh5::Source %{
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

%include <brfc/oh5/Scalar.hpp>
%include <brfc/oh5/Node.hpp>
%include <brfc/oh5/Group.hpp>
%include <brfc/oh5/AttributeGroup.hpp>
%include <brfc/oh5/RootGroup.hpp>
%include <brfc/oh5/Attribute.hpp>
%include <brfc/oh5/Source.hpp>
%include <brfc/oh5/File.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
