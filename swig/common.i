
#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "boost_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "QString.i"


%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
%}

%exception {
    try {
        $action
    } catch (const brfc::db_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/DatabaseError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::fs_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/FileSystemError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::lookup_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/LookupError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::duplicate_entry& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/DuplicateEntry");
        jenv->ThrowNew(cls, e.what());
        return $null;
    } catch (const brfc::brfc_error& e) {
        jclass cls = jenv->FindClass("eu/baltrad/fc/FileCatalogError");
        jenv->ThrowNew(cls, e.what());
        return $null;
    }
}

/* vim:filetype=cpp:et:ts=4:sw=4:
*/
