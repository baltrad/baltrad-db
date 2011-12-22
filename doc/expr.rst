Query Expressions
=================

BDB uses S-expressions to define queries because they are easy to evaluate
and transform.

Types
-----

An expression can be of the following types:

* **symbol** - UTF-8 characters
* **string** - UTF-8 characters surrounded by double quotes (*"*)
* **long** - a 64-bit integer
* **double** - a double precision floating-point number
* **boolean** - true/false
* **list** - a list of expressions

.. _doc-expr-eval:

Evaluation
----------

* Basic types (string, long, double, boolean) evaluate to themselves.
* Symbols evaluate to callable procedures.
* List are evaluated from left to right. If the first element in a list
  is a symbol, the resulting procedure is called with the evaluation
  result(s) of the rest of the elements in the list.

Procedures
----------

Arithmetic operators (+, -, \*, /)
''''''''''''''''''''''''''''''''''

Comparison operators (>, <, =, !=, <=, >=)
''''''''''''''''''''''''''''''''''''''''''

Logic operators (and, or, not)
''''''''''''''''''''''''''''''

List containment (in)
'''''''''''''''''''''

String comparison (like)
''''''''''''''''''''''''

Attribute access (attr)
'''''''''''''''''''''''


