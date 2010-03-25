%module fc_oh5

%include "common.i"
%import "fc.i"

%{
    #include <brfc/oh5/Node.hpp>
    #include <brfc/oh5/Attribute.hpp>
    #include <brfc/oh5/AttributeGroup.hpp>
    #include <brfc/oh5/AttributeSpecs.hpp>
    #include <brfc/oh5/DataGroup.hpp>
    #include <brfc/oh5/DataSetGroup.hpp>
    #include <brfc/oh5/File.hpp>
    #include <brfc/oh5/Group.hpp>
    #include <brfc/oh5/QualityGroup.hpp>
    #include <brfc/oh5/RootGroup.hpp>
    #include <brfc/oh5/Source.hpp>
    #include <brfc/oh5/SourceCentre.hpp>
    #include <brfc/oh5/SourceRadar.hpp>

    #include <QtCore/QDate>
    #include <QtCore/QTime>
%}

%ignore brfc::oh5::AttributeSpecs::converter;

%ignore brfc::oh5::Node::begin;
%ignore brfc::oh5::Node::end;

SWIG_SHARED_PTR(File, brfc::oh5::File);

SWIG_SHARED_PTR(Node, brfc::oh5::Node)
SWIG_SHARED_PTR_DERIVED(Node,
                        brfc::oh5::Node,
                        brfc::oh5::Attribute);
SWIG_SHARED_PTR_DERIVED(Group,
                        brfc::oh5::Node,
                        brfc::oh5::Group);
SWIG_SHARED_PTR_DERIVED(RootGroup,
                        brfc::oh5::Group,
                        brfc::oh5::RootGroup);
SWIG_SHARED_PTR_DERIVED(AttributeGroup,
                        brfc::oh5::Group,
                        brfc::oh5::AttributeGroup);
SWIG_SHARED_PTR_DERIVED(DataSetGroup,
                        brfc::oh5::Group,
                        brfc::oh5::DataSetGroup);
SWIG_SHARED_PTR_DERIVED(DataGroup,
                        brfc::oh5::Group,
                        brfc::oh5::DataGroup);
SWIG_SHARED_PTR_DERIVED(QualityGroup,
                        brfc::oh5::Group,
                        brfc::oh5::QualityGroup);


SWIG_SHARED_PTR(Source, brfc::oh5::Source);
SWIG_SHARED_PTR_DERIVED(SourceCentre,
                        brfc::oh5::Source,
                        brfc::oh5::SourceCentre);
SWIG_SHARED_PTR_DERIVED(SourceRadar,
                        brfc::oh5::Source,
                        brfc::oh5::SourceRadar);

%template(NodeVector) std::vector<brfc::shared_ptr<brfc::oh5::Node> >;

%typemap(javaimports) brfc::oh5::File,
                      brfc::oh5::File* %{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.StringVector;
%}

%typemap(javaimports) brfc::oh5::Attribute,
                      brfc::oh5::Attribute* %{
    import eu.baltrad.fc.Variant;
%}

%pragma(java) jniclassimports=%{
    import eu.baltrad.fc.Date;
    import eu.baltrad.fc.Time;
    import eu.baltrad.fc.Variant;
%}

// make constructors for SWIG_SHARED_PTR public
%typemap(javabody) brfc::oh5::File,
                   brfc::oh5::Source %{
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

// ignore Source members
%ignore brfc::Source::Source;
%ignore brfc::Source::from_source_attribute;

%include <brfc/oh5/Node.hpp>
%include <brfc/oh5/Group.hpp>
%include <brfc/oh5/AttributeGroup.hpp>
%include <brfc/oh5/AttributeSpecs.hpp>
%include <brfc/oh5/DataSetGroup.hpp>
%include <brfc/oh5/DataGroup.hpp>
%include <brfc/oh5/QualityGroup.hpp>
%include <brfc/oh5/RootGroup.hpp>
%include <brfc/oh5/Attribute.hpp>
%include <brfc/oh5/Source.hpp>
%include <brfc/oh5/SourceCentre.hpp>
%include <brfc/oh5/SourceRadar.hpp>
%include <brfc/oh5/File.hpp>

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
