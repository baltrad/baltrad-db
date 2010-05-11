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

#include <brfc/Url.hpp>

#include <brfc/exceptions.hpp>

#include <iostream>

namespace brfc {

Url::Url()
        : scheme_()
        , user_name_()
        , password_()
        , host_()
        , port_(0)
        , path_()
        , query_() {

}

Url::Url(const String& str)
        : scheme_()
        , user_name_()
        , password_()
        , host_()
        , port_(0)
        , path_()
        , query_() {
    parse(str);
}

Url::Url(const Url& other)
        : scheme_(other.scheme_)
        , user_name_(other.user_name_)
        , password_(other.password_)
        , host_(other.host_)
        , port_(other.port_)
        , path_(other.path_)
        , query_(other.query_) {
    
}

Url&
Url::operator=(const Url& rhs) {
    if (this != &rhs) {
        scheme_ = rhs.scheme_;
        user_name_ = rhs.user_name_;
        password_ = rhs.password_;
        host_ = rhs.host_;
        port_ = rhs.port_;
        path_ = rhs.path_;
        query_ = rhs.query_;
    }
    return *this;
}

void
Url::parse(const String& str) {
    // find required scheme
    int scheme_end = str.index_of("://");
    if (scheme_end == -1)
        throw value_error("invalid url");
    scheme(str.substr(0, scheme_end));

    scheme_end += 3; // advance past ://
    
    int path_end = str.index_of("/", scheme_end);
    if (path_end == -1) {
        // entire remaining string is the authority section
        authority(str.substr(scheme_end));
        return;
    }
    authority(str.substr(scheme_end, path_end - scheme_end));

    int query_end = str.index_of("?", path_end);
    if (query_end == -1) {
        // entire remaining string is the path
        path(str.substr(path_end));
        return;
    }
    path(str.substr(path_end, query_end - path_end));

    // remaining string is the query
    // XXX: currently not dealing with #fragment
    query(str.substr(query_end));
}

void
Url::authority(const String& authority) {
    int user_info_end = authority.index_of("@");
    if (user_info_end == -1) {
        host_info(authority);
    } else {
        user_info(authority.substr(0, user_info_end));
        host_info(authority.substr(user_info_end + 1));
    }
}

void
Url::user_info(const String& user_info) {
    int user_name_end = user_info.index_of(":");
    if (user_name_end == -1) {
        user_name(user_info);
    } else {
        user_name(user_info.substr(0, user_name_end));
        password(user_info.substr(user_name_end + 1));
    }
}

void
Url::host_info(const String& host_info) {
    int host_end = host_info.index_of(":");
    if (host_end == -1) {
        host(host_info);
    } else {
        host(host_info.substr(0, host_end));
        port(host_info.substr(host_end + 1).to_int());
    }
}

} // namespace brfc
