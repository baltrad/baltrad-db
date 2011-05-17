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

%module fc_expr

%include "common.i"

%{
    #include <sstream>
    #include <brfc/expr/AttributePrototypes.hpp>
    #include <brfc/expr/Expression.hpp>
    #include <brfc/expr/ExpressionFactory.hpp>
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
 * brfc::Expression
 */
%extend brfc::Expression {
    std::string toString() {
        std::stringstream ss;
        ss << *$self;
        return ss.str();
    }
};

%ignore brfc::Expression::Expression(const std::string& value, construct_symbol);
%ignore brfc::Expression::Expression(const list_t& );
%ignore brfc::Expression::type() const;
%ignore brfc::Expression::list() const;
%ignore brfc::Expression::begin;
%ignore brfc::Expression::end;
%ignore brfc::PrintTo;

%rename(equals) brfc::operator==(const Expression&, const Expression&);

%typemap(javaimports) brfc::Expression %{
  import java.util.Collection;

  import eu.baltrad.fc.Date;
  import eu.baltrad.fc.Time;
  import eu.baltrad.fc.DateTime;
  import eu.baltrad.fc.TimeDelta;
%}

SWIG_JAVABODY_METHODS(public, public, brfc::Expression)

%typemap(javacode) brfc::Expression %{
  public Expression(Collection<Expression> values) {
    this();
    for (Expression e : values) {
      push_back(e);
    }
  }

  public Expression(Expression[] values) {
    this();
    for (int i=0; i < values.length; i++) {
      push_back(values[i]);
    }
  }
  
  @Override
  public boolean equals(Object o) {
    if (o instanceof Expression) {
        return fc_expr.equals(this, (Expression)o);
    } else {
        return false;
    }
  }
%}

/***
 * brfc::ExpressionFactory
 */
%ignore brfc::ExpressionFactory::string(const char* value) const;
%typemap(javaimports) brfc::ExpressionFactory %{
  import java.util.Collection;

  import eu.baltrad.fc.Date;
  import eu.baltrad.fc.DateTime;
  import eu.baltrad.fc.Time;
  import eu.baltrad.fc.TimeDelta;
  import eu.baltrad.fc.Variant;
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.DateTime;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.TimeDelta;
    import eu.baltrad.fc.Variant;
%}

%include <brfc/expr/AttributePrototypes.hpp>
%include <brfc/expr/Expression.hpp>
%include <brfc/expr/ExpressionFactory.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
