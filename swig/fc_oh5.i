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

%module fc_oh5

%include "common.i"

%{
    #include <brfc/Date.hpp>
    #include <brfc/Time.hpp>
    #include <brfc/oh5/Oh5Node.hpp>
    #include <brfc/oh5/Oh5Scalar.hpp>
    #include <brfc/oh5/Oh5Attribute.hpp>
    #include <brfc/oh5/Oh5File.hpp>
    #include <brfc/oh5/Oh5FileMatcher.hpp>
    #include <brfc/oh5/PhysicalOh5File.hpp>
    #include <brfc/oh5/Oh5Group.hpp>
    #include <brfc/oh5/Oh5Source.hpp>
%}

%import "fc.i"
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
 * brfc::oh5::Oh5Node
 */
%ignore brfc::oh5::Oh5Node::Oh5Node;
%ignore brfc::oh5::Oh5Node::begin;
%ignore brfc::oh5::Oh5Node::end;
%ignore brfc::oh5::Oh5Node::add_child;
%ignore brfc::oh5::Oh5Node::backend;
%ignore brfc::oh5::Oh5Node::children() const;
%ignore brfc::oh5::Oh5Node::parent() const;
%ignore brfc::oh5::Oh5Node::root() const;
%ignore brfc::oh5::Oh5Node::child(const std::string&) const;
%ignore brfc::oh5::Oh5Node::file() const;

// ignore Oh5Node constructors (std::auto_ptr)
%ignore brfc::oh5::Oh5Attribute::Oh5Attribute;
%ignore brfc::oh5::Oh5Dataset::Oh5Dataset;
%ignore brfc::oh5::Oh5Group::Oh5Group;
%ignore brfc::oh5::Oh5Node::Oh5Node;

%template(Oh5NodeVector) std::vector<brfc::oh5::Oh5Node* >;

/***
 * brfc::oh5::Oh5Group
 */
%ignore brfc::oh5::Oh5Group::attribute(const std::string&) const;
%ignore brfc::oh5::Oh5Group::effective_attribute(const std::string&) const;
%ignore brfc::oh5::Oh5Group::group(const std::string&) const;

%typemap(javaimports) brfc::oh5::Oh5Group,
                      brfc::oh5::Oh5Group* %{
%}

/***
 * brfc::oh5::Oh5Source
 */
SWIG_JAVABODY_METHODS(public, public, brfc::oh5::Oh5Source);

%rename(_keys) brfc::oh5::Oh5Source::keys;
%rename(_all_keys) brfc::oh5::Oh5Source::all_keys;

%typemap(javaimports) brfc::oh5::Oh5Source,
                      brfc::oh5::Oh5Source* %{
    import java.util.List;
    import java.util.ArrayList;
    
    import eu.baltrad.fc._StdVectorString;
%}

%typemap(javacode) brfc::oh5::Oh5Source %{
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
 * std::vector<brfc::oh5::Oh5Source>
 */
SWIG_JAVABODY_METHODS(public, public, std::vector<brfc::oh5::Oh5Source>);
%template(_StdVectorOh5Source) std::vector<brfc::oh5::Oh5Source>;

/***
 * brfc::oh5::Oh5Scalar
 */
%ignore brfc::oh5::Oh5Scalar::Oh5Scalar(const char* value);
%ignore brfc::oh5::Oh5Scalar::operator=;

%typemap(javaimports) brfc::oh5::Oh5Scalar,
                      brfc::oh5::Oh5Scalar* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
%}


/***
 * brfc::oh5::Oh5File
 */
%ignore brfc::oh5::Oh5File::group(const brfc::string&) const;
%ignore brfc::oh5::Oh5File::root() const;

%typemap(javaimports) brfc::oh5::Oh5File,
                      brfc::oh5::Oh5File* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
%}

SWIG_JAVABODY_METHODS(public, public, brfc::oh5::Oh5File);

/***
 * brfc::oh5::PhysicalOh5File
 */
SWIG_JAVABODY_METHODS(public, public, brfc::oh5::PhysicalOh5File);

/***
 * brfc::oh5::Oh5FileMatcher
 */
%typemap(javaimports) brfc::oh5::Oh5FileMatcher %{
    import eu.baltrad.fc.expr.Expression;
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.expr.Expression;
%}

%include <brfc/oh5/Oh5Scalar.hpp>
%include <brfc/oh5/Oh5Node.hpp>
%include <brfc/oh5/Oh5Group.hpp>
%include <brfc/oh5/Oh5Attribute.hpp>
%include <brfc/oh5/Oh5Source.hpp>
%include <brfc/oh5/Oh5File.hpp>
%include <brfc/oh5/Oh5FileMatcher.hpp>
%include <brfc/oh5/PhysicalOh5File.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
