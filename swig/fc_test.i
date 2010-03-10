%module fc_test

%include "common.i"

%{
    #include <brfc/test/TestSwig.hpp>
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

%include <brfc/test/TestSwig.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
