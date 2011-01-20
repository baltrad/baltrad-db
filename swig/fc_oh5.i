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
%ignore brfc::oh5::Node::child(const std::string&) const;
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
%ignore brfc::oh5::Group::attribute(const std::string&) const;
%ignore brfc::oh5::Group::effective_attribute(const std::string&) const;
%ignore brfc::oh5::Group::group(const std::string&) const;

%typemap(javaimports) brfc::oh5::Group,
                      brfc::oh5::Group* %{
%}

/***
 * brfc::oh5::Source
 */
SWIG_JAVABODY_METHODS(public, public, brfc::oh5::Source);

%rename(_keys) brfc::oh5::Source::keys;
%rename(_all_keys) brfc::oh5::Source::all_keys;

%typemap(javaimports) brfc::oh5::Source,
                      brfc::oh5::Source* %{
    import java.util.List;
    import java.util.ArrayList;
    
    import eu.baltrad.fc._StdVectorString;
%}

%typemap(javacode) brfc::oh5::Source %{
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
 * std::vector<brfc::oh5::Source>
 */
SWIG_JAVABODY_METHODS(public, public, std::vector<brfc::oh5::Source>);
%template(_StdVectorSource) std::vector<brfc::oh5::Source>;

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
 * brfc::oh5::File
 */
%ignore brfc::oh5::File::group(const brfc::string&) const;
%ignore brfc::oh5::File::root() const;

%typemap(javaimports) brfc::oh5::File,
                      brfc::oh5::File* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
%}

SWIG_JAVABODY_METHODS(public, public, brfc::oh5::File);

/***
 * brfc::oh5::PhysicalFile
 */
SWIG_JAVABODY_METHODS(public, public, brfc::oh5::PhysicalFile);

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
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
