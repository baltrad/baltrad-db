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

#include <string>

namespace brfc {

/**
 * @brief a naive URL implementation
 *
 * <scheme>://<user_name>:<password>@<host>:<port>/<path>?<query>
 *
 * @note currently doesn't support encoded URLs (this might be problematic
 *       for passwords containing '/', '@', ':', etc.)
 * @note scheme is always followed by '://' not ':'
 * @note there is not difference between an empty and an unspecified password
 */
class Url {
  public:
    Url();

    explicit Url(const std::string& str);

    Url(const Url& other);

    Url& operator=(const Url& rhs);
    
    /**
     * @brief set user_name, password, host and port from authority string
     */
    void authority(const std::string& authority);
    
    /**
     * @brief set user_name and password
     */
    void user_info(const std::string& user_info);
    
    /**
     * @brief set host and port
     */
    void host_info(const std::string& host_info);

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
     * @brief access query
     *
     * @note don't use this. Has to be rewritten as a String:String map
     */
    const std::string& query() const {
        return query_;
    }

    void query(const std::string& query) {
        query_ = query;
    }

  private:
    void parse(const std::string& str);

    std::string scheme_;
    std::string user_name_;
    std::string password_;
    std::string host_;
    int port_;
    std::string path_;
    std::string query_;
};

} // namespace brfc

#endif // BRFC_URL_HPP
