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

#define SWIG_SHARED_PTR_NAMESPACE brfc
%include "boost_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "String.i"


%{
    #include <brfc/exceptions.hpp>
    #include <brfc/smart_ptr.hpp>
%}

%ignore boost::noncopyable;
namespace boost {
    class noncopyable {};
}

#define DEPRECATED(func) func

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
