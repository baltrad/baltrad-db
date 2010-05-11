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

#include <brfc/String.hpp>

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

    explicit Url(const String& str);

    Url(const Url& other);

    Url& operator=(const Url& rhs);
    
    /**
     * @brief set user_name, password, host and port from authority string
     */
    void authority(const String& authority);
    
    /**
     * @brief set user_name and password
     */
    void user_info(const String& user_info);
    
    /**
     * @brief set host and port
     */
    void host_info(const String& host_info);

    const String& scheme() const {
        return scheme_;
    }

    void scheme(const String& scheme) {
        scheme_ = scheme;
    }

    const String& user_name() const {
        return user_name_;
    }

    void user_name(const String& user_name) {
        user_name_ = user_name;
    }

    const String& password() const {
        return password_;
    }

    void password(const String& password) {
        password_ = password;
    }

    const String& host() const {
        return host_;
    }

    void host(const String& host) {
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

    const String& path() const {
        return path_;
    }

    void path(const String& path) {
        path_ = path;
    }
    
    /**
     * @brief access query
     *
     * @note don't use this. Has to be rewritten as a String:String map
     */
    const String& query() const {
        return query_;
    }

    void query(const String& query) {
        query_ = query;
    }

  private:
    void parse(const String& str);

    String scheme_;
    String user_name_;
    String password_;
    String host_;
    int port_;
    String path_;
    String query_;
};

} // namespace brfc

#endif // BRFC_URL_HPP
