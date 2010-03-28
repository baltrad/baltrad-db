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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/


%{
#include <QtCore/QString>
%}

%naturalvar QString;

class QString;

// string
%typemap(jni) QString "jstring"
%typemap(jtype) QString "String"
%typemap(jstype) QString "String"

/* from Java to C++ */
%typemap(in) QString
%{
    if (!$input) {
       SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                               "null std::string");
       return $null;
    } 
    const jchar* $1_pstr = jenv->GetStringChars($input, 0);
    if (!$1_pstr) return $null;
    $1.setUtf16($1_pstr, jenv->GetStringLength($input));
    jenv->ReleaseStringChars($input, $1_pstr);
%}

/* from C++ to Java */
%typemap(out) QString
%{ $result = jenv->NewString((const jchar*)$1.unicode(), (jsize)$1.length()); %}

%typemap(javain) QString "$javainput"

%typemap(javaout) QString {
    return $jnicall;
  }

/* const QString& */
%typemap(jni) const QString& "jstring"
%typemap(jtype) const QString& "String"
%typemap(jstype) const QString& "String"

/* from Java to C++ */
%typemap(in) const QString&
%{
    if(!$input) {
        SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException,
                                "null std::string");
        return $null;
    }
    const jchar *$1_pstr = jenv->GetStringChars($input, 0); 
    if (!$1_pstr) return $null;
    QString $1_qstr = QString::fromUtf16($1_pstr, jenv->GetStringLength($input));
    $1 = &$1_qstr;
    jenv->ReleaseStringChars($input, $1_pstr);
%}

/* from C++ to Java */
%typemap(out) const QString& 
%{ $result = jenv->NewString((const jchar*)$1->unicode(), (jsize)$1->length()); %}

%typemap(javain) const QString& "$javainput"

%typemap(javaout) const QString& {
    return $jnicall;
}
