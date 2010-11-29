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

%{
    #include <brfc/Date.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/StringList.hpp>
    #include <brfc/oh5/Node.hpp>
    #include <brfc/oh5/Scalar.hpp>
    #include <brfc/oh5/Attribute.hpp>
    #include <brfc/oh5/File.hpp>
    #include <brfc/oh5/PhysicalFile.hpp>
    #include <brfc/oh5/Group.hpp>
    #include <brfc/oh5/RootGroup.hpp>
    #include <brfc/oh5/Source.hpp>
%}

%import "fc.i"

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
 * brfc::oh5::Node
 */
%ignore brfc::oh5::Node::Node;
%ignore brfc::oh5::Node::begin;
%ignore brfc::oh5::Node::end;
%ignore brfc::oh5::Node::add_child;
%ignore brfc::oh5::Node::backend;
%ignore brfc::oh5::Node::children() const;
%ignore brfc::oh5::Node::parent() const;
%ignore brfc::oh5::Node::root() const;
%ignore brfc::oh5::Node::child(const brfc::String&) const;
%ignore brfc::oh5::Node::file() const;

// ignore Node constructors (std::auto_ptr)
%ignore brfc::oh5::Attribute::Attribute;
%ignore brfc::oh5::Dataset::Dataset;
%ignore brfc::oh5::Group::Group;
%ignore brfc::oh5::Node::Node;
%ignore brfc::oh5::RootGroup::RootGroup;

%template(NodeVector) std::vector<brfc::oh5::Node* >;

/***
 * brfc::oh5::Group
 */
%ignore brfc::oh5::Group::attribute(const brfc::String&) const;
%ignore brfc::oh5::Group::effective_attribute(const brfc::String&) const;
%ignore brfc::oh5::Group::group(const brfc::String&) const;

%typemap(javaimports) brfc::oh5::Group,
                      brfc::oh5::Group* %{
    import eu.baltrad.fc.StringList;
%}

/***
 * brfc::oh5::Source
 */
%typemap(javaimports) brfc::oh5::Source,
                      brfc::oh5::Source* %{
    import eu.baltrad.fc.StringList;
%}

%typemap(javabody) brfc::oh5::Source, std::vector<brfc::oh5::Source> %{
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

%template(SourceVector) std::vector<brfc::oh5::Source>;


/***
 * brfc::oh5::Scalar
 */
%ignore brfc::oh5::Scalar::Scalar(const char* value);
%ignore brfc::oh5::Scalar::operator=;

%typemap(javaimports) brfc::oh5::Scalar,
                      brfc::oh5::Scalar* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
%}


/***
 * brfc::oh5::File;
 */
SWIG_SHARED_PTR(File, brfc::oh5::File);
SWIG_SHARED_PTR_DERIVED(PhysicalFile,
                        brfc::oh5::File,
                        brfc::oh5::PhysicalFile);

%ignore brfc::oh5::File::group(const brfc::String&) const;
%ignore brfc::oh5::File::root() const;

%typemap(javaimports) brfc::oh5::File,
                      brfc::oh5::File* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.StringList;
%}

%typemap(javabody) brfc::oh5::File %{
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

%typemap(javabody_derived) brfc::oh5::PhysicalFile %{
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

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.StringList;
%}

%include <brfc/oh5/Scalar.hpp>
%include <brfc/oh5/Node.hpp>
%include <brfc/oh5/Group.hpp>
%include <brfc/oh5/RootGroup.hpp>
%include <brfc/oh5/Attribute.hpp>
%include <brfc/oh5/Source.hpp>
%include <brfc/oh5/File.hpp>
%include <brfc/oh5/PhysicalFile.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
