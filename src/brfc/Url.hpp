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

#ifndef BRFC_URL_HPP
#define BRFC_URL_HPP

#include <map>
#include <string>

namespace brfc {

/**
 * @brief a naive URL implementation
 *
 * this follows the common internet scheme syntax of RFC-1738:
 * <scheme>://<user_name>:<password>@<host>:<port>/<path>
 *
 * @note currently doesn't support encoded URLs (this might be problematic
 *       for passwords containing '/', '@', ':', etc.)
 */
class Url {
  public:
    Url();

    Url(const std::string& str);

    Url(const Url& other);

    Url& operator=(const Url& rhs);
    
    const std::string& scheme() const {
        return scheme_;
    }

    void scheme(const std::string& scheme) {
        scheme_ = scheme;
    }

    const std::string& user_name() const {
        return user_name_;
    }

    void user_name(const std::string& user_name) {
        user_name_ = user_name;
    }

    const std::string& password() const {
        return password_;
    }

    void password(const std::string& password) {
        password_ = password;
    }

    const std::string& host() const {
        return host_;
    }

    void host(const std::string& host) {
        host_ = host;
    }

    /**
     * @brief port or 0 if unspecified
     */
    int port() const {
        return port_;
    }

    void port(int port) {
        port_ = port;
    }

    const std::string& path() const {
        return path_;
    }

    void path(const std::string& path) {
        path_ = path;
    }
    
    /**
     * @brief parse http path from path()
     *
     * for a path path/to/rsrc?arg1=val1&arg2=val2 return path/to/rsrc
     */
    std::string http_path() const;
    
    /**
     * @brief parse http searchpart from path()
     *
     * for a path path/to/rsrc?arg1=val1&arg2=val2 return a map
     * {arg1: val1, arg2: val2}
     */
    std::map<std::string, std::string> http_searchpart() const;
    
  private:
    void parse(const std::string& str);

    std::string scheme_;
    std::string user_name_;
    std::string password_;
    std::string host_;
    int port_;
    std::string path_;
};

} // namespace brfc

#endif // BRFC_URL_HPP
