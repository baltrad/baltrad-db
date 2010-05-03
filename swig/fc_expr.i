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

%module fc_expr

%include "common.i"

%{
    #include <brfc/expr/fwd.hpp>
    #include <brfc/expr/Element.hpp>
    #include <brfc/expr/Expression.hpp>
    #include <brfc/expr/Factory.hpp>
    #include <brfc/expr/Attribute.hpp>
    #include <brfc/expr/Parentheses.hpp>
    #include <brfc/expr/Label.hpp>
    #include <brfc/expr/BinaryOperator.hpp>
    #include <brfc/expr/Literal.hpp>
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

%ignore brfc::expr::Visitor;
%ignore brfc::expr::Compiler;
%ignore brfc::expr::Select;

%ignore brfc::expr::Element::accept;
%ignore brfc::expr::Expression::shared_from_this;
%ignore brfc::expr::Factory::string(const char* value) const;
%ignore brfc::expr::Literal::create;
%ignore brfc::expr::Literal::value;
%ignore brfc::expr::BinaryOperator::create;
%ignore brfc::expr::Attribute::create;
%ignore brfc::expr::Parentheses::create;
%ignore brfc::expr::Label::create;

%rename(ExpressionFactory) brfc::expr::Factory;
%rename(AttributeExpr) brfc::expr::Attribute;
%rename(boolean_) brfc::expr::Factory::boolean;

SWIG_SHARED_PTR(Element,
                brfc::expr::Element)
SWIG_SHARED_PTR_DERIVED(Expression,
                        brfc::expr::Element,
                        brfc::expr::Expression);
SWIG_SHARED_PTR_DERIVED(BinaryOperator,
                        brfc::expr::Expression,
                        brfc::expr::BinaryOperator);
SWIG_SHARED_PTR_DERIVED(BinaryOperator,
                        brfc::expr::Expression,
                        brfc::expr::BinaryOperator);
SWIG_SHARED_PTR_DERIVED(Literal,
                        brfc::expr::Expression,
                        brfc::expr::Literal);
SWIG_SHARED_PTR_DERIVED(Label,
                        brfc::expr::Expression,
                        brfc::expr::Label);
SWIG_SHARED_PTR_DERIVED(Parentheses,
                        brfc::expr::Expression,
                        brfc::expr::Parentheses);
SWIG_SHARED_PTR_DERIVED(AttributeExpr,
                        brfc::expr::Expression,
                        brfc::expr::Attribute);

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

%template(AttributeExprVector) std::vector<brfc::shared_ptr<brfc::expr::Attribute> >;


%typemap(javabody_derived) brfc::expr::Expression %{
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

%include <brfc/expr/fwd.hpp>
%include <brfc/expr/Element.hpp>
%include <brfc/expr/Expression.hpp>
%include <brfc/expr/Label.hpp>
%include <brfc/expr/Factory.hpp>
%include <brfc/expr/Attribute.hpp>
%include <brfc/expr/Parentheses.hpp>
%include <brfc/expr/BinaryOperator.hpp>
%include <brfc/expr/Literal.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
