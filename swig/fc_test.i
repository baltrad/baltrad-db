%module fc_test

%include "common.i"
%import "fc_oh5.i"

%{
    #include <brfc/test/TestSwig.hpp>
    #include <brfc/test/TestRDB.hpp>
    #include <brfc/test/TempH5File.hpp>
    #include <brfc/test/TempDir.hpp>
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

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.oh5.File;
%}

%typemap(javaimports) brfc::test::TempH5File %{
    import eu.baltrad.fc.oh5.File;
%}

%include <brfc/test/TestSwig.hpp>
%include <brfc/test/TestRDB.hpp>
%include <brfc/test/TempH5File.hpp>
%include <brfc/test/TempDir.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
