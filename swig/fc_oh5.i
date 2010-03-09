%module fc_oh5

%include "common.i"
%import "fc.i"

%{
    #include <brfc/oh5/Node.hpp>
    #include <brfc/oh5/Attribute.hpp>
    #include <brfc/oh5/AttributeGroup.hpp>
    #include <brfc/oh5/File.hpp>
    #include <brfc/oh5/Group.hpp>
    #include <brfc/oh5/Root.hpp>

    #include <QtCore/QDate>
    #include <QtCore/QTime>
%}

SWIG_SHARED_PTR(File, brfc::oh5::File);

SWIG_SHARED_PTR(Node, brfc::oh5::Node)
SWIG_SHARED_PTR_DERIVED(Node,
                        brfc::oh5::Node,
                        brfc::oh5::Attribute);
SWIG_SHARED_PTR_DERIVED(Group,
                        brfc::oh5::Node,
                        brfc::oh5::Group);
SWIG_SHARED_PTR_DERIVED(Root,
                        brfc::oh5::Group,
                        brfc::oh5::Root);
SWIG_SHARED_PTR_DERIVED(AttributeGroup,
                        brfc::oh5::Group,
                        brfc::oh5::AttributeGroup);

%template(NodeVector) std::vector<brfc::shared_ptr<brfc::oh5::Node> >;

%include <brfc/oh5/Node.hpp>
%include <brfc/oh5/Group.hpp>
%include <brfc/oh5/AttributeGroup.hpp>
%include <brfc/oh5/Root.hpp>
%include <brfc/oh5/Attribute.hpp>
%include <brfc/oh5/File.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
