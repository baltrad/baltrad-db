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

#include <brfc/util/Url.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/xpressive/xpressive_static.hpp>
#include <boost/xpressive/regex_actions.hpp>

#include <brfc/exceptions.hpp>

namespace brfc {

Url::Url()
        : scheme_()
        , user_name_()
        , password_()
        , host_()
        , port_(0)
        , path_() {

}

Url::Url(const std::string& str)
        : scheme_()
        , user_name_()
        , password_()
        , host_()
        , port_(0)
        , path_() {
    parse(str);
}

Url::Url(const Url& other)
        : scheme_(other.scheme_)
        , user_name_(other.user_name_)
        , password_(other.password_)
        , host_(other.host_)
        , port_(other.port_)
        , path_(other.path_) {
    
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
    }
    return *this;
}

void
Url::parse(const std::string& str) {
    using namespace boost::xpressive;

    mark_tag t_scheme(1), t_user(2), t_passwd(3), t_host(4), t_port(5), t_path(6);
    sregex re = 
        (t_scheme = +(alpha)) >> "://" >> 
        !((t_user=+~(set=':','@')) >>
          !(":" >> (t_passwd=+~(set='@'))) >> "@") >>
        ((t_host=+~(set=':','/')) >>
         !(":" >> (t_port=+(digit)))) >> 
        !("/" >> (t_path = *_));

    smatch match;
    if (not regex_match(str, match, re)) {
        throw std::invalid_argument("invalid url");
    }

    scheme(match[t_scheme]);
    user_name(match[t_user]);
    password(match[t_passwd]);
    host(match[t_host]);
    if (match[t_port])
        port(boost::lexical_cast<int>(match[t_port]));
    path(match[t_path]);
}

std::string
Url::http_path() const {
    return path_.substr(0, path_.find('?'));
}

std::map<std::string, std::string>
Url::http_searchpart() const {
    using namespace boost::xpressive;

    std::map<std::string, std::string> spart;

    sregex pair = ((s1=+_w) >> "=" >> (s2=*_w)) [ref(spart)[s1] = s2];
    sregex re = "?" >> pair >> *("&" >> pair);

    regex_search(path_, re);

    return spart;
}

} // namespace brfc
