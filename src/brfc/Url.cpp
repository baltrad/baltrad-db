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

#include <iostream>

#include <boost/lexical_cast.hpp>

#include <brfc/exceptions.hpp>

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

Url::Url(const std::string& str)
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
Url::parse(const std::string& str) {
    // find required scheme
    size_t scheme_end = str.find("://");
    if (scheme_end == std::string::npos)
        throw value_error("invalid url");
    scheme(str.substr(0, scheme_end));

    scheme_end += 3; // advance past ://
    
    size_t path_end = str.find("/", scheme_end);
    if (path_end == std::string::npos) {
        // entire remaining string is the authority section
        authority(str.substr(scheme_end));
        return;
    }
    authority(str.substr(scheme_end, path_end - scheme_end));

    size_t query_end = str.find("?", path_end);
    if (query_end == std::string::npos) {
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
Url::authority(const std::string& authority) {
    size_t user_info_end = authority.find("@");
    if (user_info_end == std::string::npos) {
        host_info(authority);
    } else {
        user_info(authority.substr(0, user_info_end));
        host_info(authority.substr(user_info_end + 1));
    }
}

void
Url::user_info(const std::string& user_info) {
    size_t user_name_end = user_info.find(":");
    if (user_name_end == std::string::npos) {
        user_name(user_info);
    } else {
        user_name(user_info.substr(0, user_name_end));
        password(user_info.substr(user_name_end + 1));
    }
}

void
Url::host_info(const std::string& host_info) {
    size_t host_end = host_info.find(":");
    if (host_end == std::string::npos) {
        host(host_info);
    } else {
        host(host_info.substr(0, host_end));
        port(boost::lexical_cast<int>(host_info.substr(host_end + 1)));
    }
}

} // namespace brfc
