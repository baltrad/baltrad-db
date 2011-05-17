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

%module fc_test

%include "common.i"

%{
    #include <brfc/test/TestRDB.hpp>
    #include <brfc/test/TempH5File.hpp>
    #include <brfc/test/TempDir.hpp>
    #include <brfc/test/misc.hpp>
%}

%import "fc_oh5.i"

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

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.oh5.Oh5File;
%}

%typemap(javaimports) brfc::test::TempH5File %{
    import eu.baltrad.fc.oh5.Oh5File;
%}

%include <brfc/test/TestRDB.hpp>
%include <brfc/test/TempH5File.hpp>
%include <brfc/test/TempDir.hpp>
%include <brfc/test/misc.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
