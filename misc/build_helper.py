# Copyright 2010 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import sys
import urlparse

__all__ = ["urlsplit"]

urlparse.uses_netloc.append("postgresql")

def urlsplit(url):
    if sys.version_info < (2, 10):
        return SplitResult(*urlparse.urlsplit(url))
    else:
        return urlparse.urlsplit(url)

# from python2.5
class BaseResult(tuple):
    """Base class for the parsed result objects.

    This provides the attributes shared by the two derived result
    objects as read-only properties.  The derived classes are
    responsible for checking the right number of arguments were
    supplied to the constructor.

    """

    __slots__ = ()

    # Attributes that access the basic components of the URL:

    @property
    def scheme(self):
        return self[0]

    @property
    def netloc(self):
        return self[1]

    @property
    def path(self):
        return self[2]

    @property
    def query(self):
        return self[-2]

    @property
    def fragment(self):
        return self[-1]

    # Additional attributes that provide access to parsed-out portions
    # of the netloc:

    @property
    def username(self):
        netloc = self.netloc
        if "@" in netloc:
            userinfo = netloc.split("@", 1)[0]
            if ":" in userinfo:
                userinfo = userinfo.split(":", 1)[0]
            return userinfo
        return None

    @property
    def password(self):
        netloc = self.netloc
        if "@" in netloc:
            userinfo = netloc.split("@", 1)[0]
            if ":" in userinfo:
                return userinfo.split(":", 1)[1]
        return None

    @property
    def hostname(self):
        netloc = self.netloc
        if "@" in netloc:
            netloc = netloc.split("@", 1)[1]
        if ":" in netloc:
            netloc = netloc.split(":", 1)[0]
        return netloc.lower() or None

    @property
    def port(self):
        netloc = self.netloc
        if "@" in netloc:
            netloc = netloc.split("@", 1)[1]
        if ":" in netloc:
            port = netloc.split(":", 1)[1]
            return int(port, 10)
        return None

class SplitResult(BaseResult):

    __slots__ = ()

    def __new__(cls, scheme, netloc, path, query, fragment):
        return BaseResult.__new__(
            cls, (scheme, netloc, path, query, fragment))

    def geturl(self):
        return urlunsplit(self)
