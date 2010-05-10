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


%{
#include <brfc/String.hpp>
%}

%naturalvar brfc::String;

/**
 * brfc::String
 */

/* JNI C type */
%typemap(jni) brfc::String "jstring"
/* Java intermediary type */
%typemap(jtype) brfc::String "String"
/* Java type */
%typemap(jstype) brfc::String "String"
/* director arguments from jtype to jstype */
%typemap(javadirectorin) brfc::String "$jniinput"
/* director return value from jstype to jtype */
%typemap(javadirectorout) brfc::String "$javacall"

/* from Java to C++ */
%typemap(in) brfc::String
%{
    if (!$input) {
        SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                                "null brfc::String");
        return $null;
    } 
    const jchar* $1_pstr = jenv->GetStringChars($input, 0);
    if (!$1_pstr) return $null;
    $1.set_utf16($1_pstr, jenv->GetStringLength($input));
    jenv->ReleaseStringChars($input, $1_pstr);
%}

/* director return value from JNI type to C++ type */
%typemap(directorout) brfc::String
%{
    if (!$input) {
        SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                                "null brfc::String");
        return $null;
    } 
    const jchar* $1_pstr = jenv->GetStringChars($input, 0);
    if (!$1_pstr) return $null;
    $result.set_utf16($1_pstr, jenv->GetStringLength($input));
    jenv->ReleaseStringChars($input, $1_pstr);
%}

/* director arguments from C++ type to JNI type */
%typemap(directorin, descriptor="Ljava/lang/String;") brfc::String 
%{
    $input = jenv->NewString((const jchar*)$1.utf16(), (jsize)$1.length());
%}

/* from C++ to Java */
%typemap(out) brfc::String
%{
    $result = jenv->NewString((const jchar*)$1.utf16(), (jsize)$1.length());
%}

/* arguments from jstype to jtype */
%typemap(javain) brfc::String "$javainput"

/* return value from jtype to jstype */
%typemap(javaout) brfc::String {
    return $jnicall;
}

/**
 * const brfc::String&
 */
%typemap(jni) const brfc::String& "jstring"
%typemap(jtype) const brfc::String& "String"
%typemap(jstype) const brfc::String& "String"

/* from Java to C++ */
%typemap(in) const brfc::String&
%{
    if (!$input) {
        SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                                "null brfc::String");
        return $null;
    }
    const jchar *$1_pstr = jenv->GetStringChars($input, 0); 
    if (!$1_pstr) return $null;
    brfc::String $1_qstr = brfc::String::from_utf16($1_pstr, jenv->GetStringLength($input));
    $1 = &$1_qstr;
    jenv->ReleaseStringChars($input, $1_pstr);
%}

/* from C++ to Java */
%typemap(out) const brfc::String& 
%{ $result = jenv->NewString((const jchar*)$1->utf16(), (jsize)$1->length()); %}

%typemap(javain) const brfc::String& "$javainput"

%typemap(javaout) const brfc::String& {
    return $jnicall;
}

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
