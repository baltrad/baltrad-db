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
    #include <brfc/expr/fwd.hpp>
    #include <brfc/expr/AttributePrototypes.hpp>
    #include <brfc/expr/Expression.hpp>
    #include <brfc/expr/ExpressionFactory.hpp>
    #include <brfc/expr/ExpressionList.hpp>
    #include <brfc/expr/Function.hpp>
    #include <brfc/expr/Attribute.hpp>
    #include <brfc/expr/Parentheses.hpp>
    #include <brfc/expr/BinaryOperator.hpp>
    #include <brfc/expr/Literal.hpp>
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

%ignore brfc::expr::Visitor;
%ignore brfc::expr::Compiler;
%ignore brfc::expr::Select;

%ignore brfc::expr::Expression::shared_from_this;
%ignore brfc::expr::Function::args;
%ignore brfc::expr::Literal::value;

%typemap(javaimports) brfc::expr::Literal %{
    import eu.baltrad.fc.Variant;
%}

/***
 * brfc::expr::ExpressionFactory
 */
%ignore brfc::expr::ExpressionFactory::string(const char* value) const;
%typemap(javaimports) brfc::expr::ExpressionFactory %{
  import java.util.Collection;

  import eu.baltrad.fc.Date;
  import eu.baltrad.fc.DateTime;
  import eu.baltrad.fc.Time;
  import eu.baltrad.fc.TimeDelta;
%}

%typemap(javacode) brfc::expr::ExpressionFactory %{
  public Expression and_(Collection<? extends Expression> values) {
    return this.and_(new ExpressionList(values));
  }

  public Expression and_(Expression[] values) {
    return this.and_(new ExpressionList(values));
  }

  public Expression or_(Collection<? extends Expression> values) {
    return this.or_(new ExpressionList(values));
  }

  public Expression or_(Expression[] values) {
    return this.or_(new ExpressionList(values));
  }
%}

SWIG_SHARED_PTR(Expression,
                brfc::expr::Expression)
SWIG_SHARED_PTR_DERIVED(BinaryOperator,
                        brfc::expr::Expression,
                        brfc::expr::BinaryOperator);
SWIG_SHARED_PTR_DERIVED(Function,
                        brfc::expr::Expression,
                        brfc::expr::Function);
SWIG_SHARED_PTR_DERIVED(Literal,
                        brfc::expr::Expression,
                        brfc::expr::Literal);
SWIG_SHARED_PTR_DERIVED(Parentheses,
                        brfc::expr::Expression,
                        brfc::expr::Parentheses);
SWIG_SHARED_PTR_DERIVED(Attribute,
                        brfc::expr::Expression,
                        brfc::expr::Attribute);
SWIG_SHARED_PTR_DERIVED(ExpressionList,
                        brfc::expr::Expression,
                        brfc::expr::ExpressionList);

%typemap(javabody) std::vector<brfc::shared_ptr<brfc::expr::Attribute> > %{
                   
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

// make constructors for SWIG_SHARED_PTR public
%typemap(javabody) brfc::expr::Expression %{
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

%typemap(javaimports) brfc::expr::Expression %{
  import java.util.Collection;
%}

%typemap(javacode) brfc::expr::Expression %{
  public BinaryOperator in(Collection<? extends Expression> values) {
    return this.in(new ExpressionList(values));
  }

  public BinaryOperator in(Expression[] values) {
    return this.in(new ExpressionList(values));
  }

  public BinaryOperator not_in(Collection<? extends Expression> values) {
    return this.not_in(new ExpressionList(values));
  }

  public BinaryOperator not_in(Expression[] values) {
    return this.not_in(new ExpressionList(values));
  }
%}


/**
 * brfc::expr::ExpressionList
 */
%typemap(javaimports) brfc::expr::ExpressionList %{
  import java.util.Collection;
%}

%typemap(javacode) brfc::expr::ExpressionList %{
  public ExpressionList(Collection<? extends Expression> values) {
    this();
    for (Expression e : values) {
      append(e);
    }
  }

  public ExpressionList(Expression[] values) {
    this();
    for (int i=0; i < values.length; i++) {
      append(values[i]);
    }
  }
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.DateTime;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.TimeDelta;
    import eu.baltrad.fc.Variant;
%}

%include <brfc/expr/fwd.hpp>
%include <brfc/expr/AttributePrototypes.hpp>
%include <brfc/expr/Expression.hpp>
%include <brfc/expr/ExpressionFactory.hpp>
%include <brfc/expr/ExpressionList.hpp>
%include <brfc/expr/Function.hpp>
%include <brfc/expr/Attribute.hpp>
%include <brfc/expr/Parentheses.hpp>
%include <brfc/expr/BinaryOperator.hpp>
%include <brfc/expr/Literal.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
